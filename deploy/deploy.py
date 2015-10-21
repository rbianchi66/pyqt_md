#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import with_statement

import sys
import os
import shutil
from subprocess2 import Popen, PIPE, STDOUT
from platform import architecture
from common import *
import genbuildnr
import imp
import tarfile
import tempfile
from glob import glob

temp_dir = join(deploy_dir, "temp")
dist_dir = join(deploy_dir, "dist")
src2_dir = join(temp_dir, "src_mirror")
extras_dir = join(root_dir, "extras")

# Configurazione (parsata dal file specificato)
cfg = dict(
    force=False,
    post_install=[],
)

# Configurazione di default per un singolo eseguibile
default_exe_cfg = dict(
    crypt=True,
    console=False,
    excludes=[],
    icons=[],
    extras_dir=extras_dir,
)

optimize_flag = {0: "", 1: "-O", 2: "-OO"}[sys.flags.optimize]

def genbuildnumber(quick):
    trace("Generate build number")

    # Genera un build number attuale e controlla che il build number sia pulito.
    os.chdir(root_dir)
    genbuildnr.GenerateBuildNrModule(join(temp_dir, "_buildnr.py"))
    buildnr = imp.load_source("_buildnr", join(temp_dir, "_buildnr.py"))
    if not buildnr.strictRevNum() and not cfg["force"]:
        print
        print "ERRORE: la working copy non e' in uno stato pulito e consistente."
        print "Controllare di non avere modifiche locali, poi lanciare un update dalla directory principale e riprovare."
        print "Numero di versione riportato da svnversion: %r" % buildnr.revnum
        sys.exit(2)

    if buildnr.major_release_date is None:
        if buildnr.path == 'trunk' and cfg['force']:
            print
            print "ATTENZIONE: il software rilasciato *NON* avrà il controllo sulla data della maintenance"
            print "=" * 79
        else:
            print
            print "ERRORE: compilare il campo 'major_release_date' in deploy.cfg per attivare il controllo sulla scadenza della maintenance"
            print "=" * 79
            sys.exit(2)

    # Prende il numero di versione. In caso di versione non finale, aggiunge
    # anche il numero di build.
    cfg["infos"]["base_version"] = buildnr.versionString()
    cfg["infos"]["version"] = cfg["infos"]["base_version"]
    if not buildnr.final:
        cfg["infos"]["version"] += "-r" + buildnr.revnum.replace(":", "+")
    if quick:
        cfg["infos"]["version"] += "Q"

# XXX: la fixDistribPath viene chiamata per trattare il dizionario distrib solo
# sotto Linux e Mac (posix), non nel caso Windows; queste due copy quasi uguali
# esistono per gestire i due casi.

def _copy_win(src, dst):
    if isdir(src):
        assert not exists(dst)
        shutil.copytree(src, dst)
    else:
        if not isdir(dst):
            os.makedirs(dst)
        for s in glob(src):
            shutil.copy2(s, dst)

def _copy_posix(src, dst):
    if isdir(src):
        assert not exists(dst)
        shutil.copytree(src, dst)
    else:
        if not isdir(dirname(dst)):
            os.makedirs(dirname(dst))

        shutil.copy2(src, dst)

def mirror_tree(quick):
    import shutil
    trace("Mirroring source tree")

    os.chdir(src_dir)
    if not quick and isdir(src2_dir):
        shutil.rmtree(src2_dir)
    if not isdir(src2_dir):
        os.makedirs(src2_dir)

    # Forziamo il locale a inglese, perché parseremo l'output di svn e non
    # vogliamo errori dovuti alle traduzioni.
    env = os.environ.copy()
    env["LC_MESSAGES"] = "C"

    dirs = ["."]

    # Trova gli eventuali external presenti sotto src_dir
    dirs.extend(genbuildnr.getExternals(".", env))

    # Esegue svn info ricorsivamente per iterare su tutti i file versionati.
    for D in dirs:
        infos = {}
        for L in Popen(["svn", "info", "--recursive", D], stdout=PIPE, env=env).stdout:
            if L.strip():
                k,v = L.strip().split(": ", 1)
                infos[k] = v
            else:
                if infos["Schedule"] == "delete":
                    continue
                fn = infos["Path"]
                infos = {}
                if fn == ".":
                    continue
                fn1 = join(src_dir, fn)
                fn2 = join(src2_dir, fn)
                if isdir(fn1):
                    if not isdir(fn2):
                        os.makedirs(fn2)
                elif not quick or newer(fn1, fn2):
                    shutil.copy2(fn1, fn2)

def build_modules(quick=False):
    trace("Building modules in deploy mode")

    builddir = join(temp_dir, "build")
    if not quick and isdir(builddir):
        shutil.rmtree(builddir)

    # Rigenera il progetto in modalità deploy
    os.chdir(root_dir)
    args = ["--deploy", builddir,
            "--source", src2_dir]
    if opts.mingw32:
        args.append("--mingw32")
    system(sys.executable, optimize_flag, join(deploy_dir, "build.py"), *args)

def _template_engine(d, conv=repr):
    import re

    def repl(m):
        if m.group(1) not in d:
            return "@ERROR@"
        return conv(d[m.group(1)])

    def process(L):
        # Sostituisce tutti i template che matchano "@nome@".
        L2 = re.sub(r"@(.*?)@", repl, L)

        # Carattere speciale ";#" in cima ad una linea: linea opzionale
        # (attiva solo se il corrispondente template viene sostituito
        # per intero)
        if L2[0:2] in (';#', '?#') and "@ERROR@" not in L2:
            L2 = "  " + L2[2:]
        # Carattere speciale "?#" in cima ad una linea: linea facoltativa
        # viene inserita solo se non ci sono errori, altrimenti è vuota
        elif L2[0:2] == "?#" and "@ERROR@" in L2:
            L2 = ""

        return L2

    def iterate(L):
        # Se c'è un template che matcha "@@nome@@", lo deve moltiplicare
        # per ogni occorrenza in input
        m = re.search(r"@@(.*?)@@", L)
        if m:
            lines = []
            for item in d[m.group(1)]:
                L = L.replace("@@%s@@" % m.group(1), item)
                lines.append(process(L))
            return lines
        else:
            return [ process(L) ]

    return iterate

def template(fn, d, conv=repr):
    import fileinput
    iterate = _template_engine(d, conv)

    for L in fileinput.FileInput(fn, inplace=True):
        lines = iterate(L)
        map(sys.stdout.write, iterate(L))

def template_str(data, d, conv=repr):
    iterate = _template_engine(d, conv)

    data = data.split('\n')
    for ix, L in enumerate(data):
        data[ix:ix+1] = iterate(L)
    return '\n'.join(data)

def create_executable(exe_cfg):
    trace("Creating executable: %s" % exe_cfg["name"])

    upx_dir = join(tools_dir, "upx")
    pyinst_dir = join(tools_dir, "PyInstaller")

    # Aggiunge la directory di UPX al Path
    os.environ["PATH"] = pathsep.join([upx_dir, os.environ["PATH"]])
    os.environ["UPX"] = "--best"

    # Copia le DLL presenti in extras dentro la directory corrente, così
    # da permettere a PyInstaller di trovarle dall'eseguibile.
    for dll in glob(join(extras_dir, "*.dll")):
        shutil.copy2(dll, temp_dir)
    shutil.copy2(join(deploy_dir, "onefile.spec"), join(temp_dir, "%s.spec" % exe_cfg["name"]))
    # Sostituisce il templating nella copia del file spec di PyInstaller
    exe_cfg["src_dir"] = src2_dir
    exe_cfg["entrypoint"] = join(src2_dir, exe_cfg["entrypoint"])
    exe_cfg["icons"] = [join(extras_dir, i) for i in exe_cfg["icons"]]
    exe_cfg["version"] = cfg["infos"]["version"]

    template(join(temp_dir, "%s.spec" % exe_cfg["name"]), exe_cfg)

    # Genera una nuova chiave
    os.chdir(temp_dir)
    system(sys.executable, join(tools_dir, "genkey.py"))

    # Lancia il build
    if not sys.platform.startswith("win"):
        system(sys.executable, "-OO", join(pyinst_dir, 'source', 'linux', "Make.py"))
        system('make', '-C', join(pyinst_dir, 'source', 'linux'))

    os.chdir(join(pyinst_dir, 'source', 'crypto'))
    system(sys.executable, "-OO", join(pyinst_dir, 'source', 'crypto', 'setup.py'), 'install')

    os.chdir(temp_dir)
    system(sys.executable, "-OO", join(pyinst_dir, "Configure.py"))

    # L'opzione -OO serve a rendere la vita più difficile a chi vuole fare
    # reverse-engineering sul programma. Gli assert più importanti nel codice
    # dovrebbero essere implementati in forma di eccezioni (come fa per
    # esempio il DB nelle check).
    system(sys.executable, "-OO",
           join(pyinst_dir, "Build.py"),
           join(temp_dir, "%s.spec" % exe_cfg["name"]))

def create_dist(output_dir):
    trace("Creating distribution setup")

    if isdir(dist_dir):
        shutil.rmtree(dist_dir)
    tree_dir = join(dist_dir, "temp")

    os.mkdir(dist_dir)
    os.mkdir(tree_dir)

    # Se nel file deploy.cfg 'distrib' (normalmente un dizionario) è posto
    # uguale a None significa che non si vuole un installer. Copiamo però
    # gli eseguibili creati nella root del progetto.
    if cfg['distrib'] is None:
        for e in cfg["executables"]:
            if sys.platform.startswith("win"):
                exe_suffix = ".exe"
            else:
                exe_suffix = ""
            exe_name = join(temp_dir, e["name"] + exe_suffix)
            trace("Copying " + exe_name + " to " + output_dir)
            _copy_win(exe_name, output_dir)
        return

    if sys.platform.startswith("win"):
        # Crea un dizionario con tutti gli eseguibili costruiti
        exefile = {}
        for e in cfg["executables"]:
            exefile[e["name"]] = join(temp_dir, e["name"] + ".exe")

        com_servers = []

        # Crea la distribuzione dentro tree_dir
        for src, dst in cfg["distrib"].items():
            if dst.startswith("SPECIAL:"):
                if dst == "SPECIAL:COM_SERVER":
                    com_servers.append(src)
                    continue
                else:
                    raise ValueError("Invalid SPECIAL flag for file: %r" % dst)
            src = exefile.get(src, src)
            dst = join(tree_dir, dst)
            src = join(src2_dir, src)
            trace("Creating distribuion for %r in %r" % (src, dst))
            _copy_win(src, dst)

        def process_installer_parms(inst, infos):
            """
            Processa il dizionario "installer" dentro il cfg e lo trasforma nella
            infos per il template di InnoSetup.
            """
            chglog = inst.pop("postinstall_changelog", None)
            if chglog:
                infos["postinstall_changelog"] = chglog
            run = inst.pop("postinstall_run", False)
            if run:
                infos["postinstall_run"] = infos["exe"]
            if inst:
                raise RuntimeError('valori sconosciuti nel dizionario "installer": %r' % inst.keys())

        # Processa il template di innosetup
        if "iss_template" in cfg.get("installer", {}):
            shutil.copy2(cfg["installer"].pop("iss_template"), join(temp_dir, "template.iss"))
        else:
            shutil.copy2(join(deploy_dir, "template.iss"), temp_dir)
        infos = cfg["infos"].copy()
        infos["name"] = cfg["name"]
        infos["exe"] = basename(exefile[cfg["executables"][0]["name"]])
        infos["tree_dir"] = tree_dir
        infos["com_server"] = com_servers
        if "installer" in cfg:
            process_installer_parms(cfg["installer"], infos)

        template(join(temp_dir, "template.iss"), infos, conv=str)

        os.chdir(dist_dir)

        system(join(deploy_dir, "tools", "InnoSetup", "ISCC.EXE"),
               "/O" + output_dir,
               join(temp_dir, "template.iss"))

    else:
        def buildDMG(output_dir):
            base_dmg = join(deploy_dir, cfg['name'])
            base_app = join(temp_dir, cfg['name'] + '.app')

            base_distrib = join(base_app, 'Contents', 'MacOS')
            base_resources = join(base_app, 'Contents', 'Resources')

            if isdir(base_dmg):
                shutil.rmtree(base_dmg)
            os.mkdir(base_dmg)

            dst_ctx = {
                'bundle': base_app,
                'resources': base_resources,
                'dmg': base_dmg,
            }
            for src, dst in cfg['distrib'].items():
                dst, isTemplate = dst
                if isTemplate:
                    # TODO: estrarre e mettere a comune il codice di gestione template
                    # presente nella parte linux
                    raise RuntimeError('TODO: implement template support in MacOS')
                dst = dst % dst_ctx
                # dst può essere un percorso assoluto, se é già
                # relativo a base_app non faccio niente (probabilmente
                # il path viene fuori dalla sostituzione di dst_ctx)
                # altrimenti gli prependo base_distrib.
                if not dst.startswith(base_app):
                    dst = base_distrib + '/' + dst

                _copy_posix(src, dst)

            # Deve contenere:
            #  * il bundle che abbiamo appena costruito;
            #  * il file .DS_Store già customizzato a mano da qualcuno - assumiamo
            #    che sia committato in extras/mac/ e lo peschiamo a colpo sicuro da lì;
            #  * lo sfondo bkg.png - assumiamo che anche questo stia in extras/mac/;
            #  * un link simbolico ad Applications;
            try:
                shutil.copy2(join(extras_dir, 'mac', 'template_ds_store'), join(base_dmg, '.DS_Store'))
                tplBkg = join(extras_dir, 'mac', 'template_bkg.png'), join(base_dmg, 'bkg.png')
                shutil.copy2(*tplBkg)
                # Nasconde il file dello sfondo
                system("/Developer/Tools/SetFile", "-a", "V", tplBkg[1])
            except IOError:
                pass
            os.symlink("/Applications", os.path.join(base_dmg, "Applications"))

            for hook in cfg['pre_package']:
                hook(**dst_ctx)

            # A questo punto impacchettiamo con hdiutil ed avremo il nostro DMG
            shutil.move(base_app, join(base_dmg, cfg['name'] + '.app'))
            dmg_fname = os.path.join(output_dir, cfg["name"] + ".dmg")
            if exists(dmg_fname):
                os.unlink(dmg_fname)
            system("hdiutil", "create", "-srcdir", base_dmg, dmg_fname)

            # La directory di base per il DMG non serve più
            #shutil.rmtree(base_dmg)

        def fixDistribPath():
            # TODO: una volta allineato il codice per windows spostare
            # la chiamata a questa funzione subito dopo il parsing della
            # configurazione
            executables = set((e['name'] for e in cfg['executables']))
            distrib = cfg['distrib']
            for src, dst in distrib.items():
                del distrib[src]
                if src in executables:
                    # ha senso indicare in distrib un exe? non me li porto
                    # dietro in ogni caso?
                    if sys.platform.startswith("win"):
                        distrib[join(temp_dir, src) + '.exe'] = (dst, False)
                    else:
                        distrib[join(temp_dir, src)] = (dst, False)
                else:
                    # la @ all'inizio del nome del file indica che lo stesso
                    # deve essere trattato come un template
                    if os.path.basename(src).startswith('@'):
                        src = join(
                            dirname(src), basename(src)[1:]
                        )
                        template = True
                    else:
                        template = False
                    if not os.path.isabs(src):
                        spath = join(src2_dir, src)
                    else:
                        spath = src
                    if exists(spath):
                        if dst and basename(src) != dst and not os.path.isabs(dst):
                            dst = join(dst, basename(src))
                        distrib[spath] = (dst, template)
                    else:
                        for f in glob(spath):
                            distrib[f] = (join(dst, basename(f)), template)

        fixDistribPath()

        if sys.platform.startswith('darwin'):
            buildDMG(output_dir)
        else:
            buildRPM(output_dir)
            buildDEB(output_dir)

class _LinuxDeploy(object):
    """
    classe con il comportamento comune tra DEBDeploy e RPMDeploy
    """
    def __init__(self, cfg, deploy_dir, temp_dir, src2_dir):
        self.deploy_dir = deploy_dir
        self.temp_dir = temp_dir
        self.src2_dir = src2_dir
        self.cfg = cfg
        self.infos = self._adaptInfos()
        self.distrib = dict(cfg['distrib'])

    def targetDir(self):
        """
        restituisce il path assoluto in cui verrà installata l'applicazione.
        l'applicazione verrà installata sotto /opt.
        """
        return join('/opt', self.infos['name'])

    def _r(self, args):
        """
        esegue un processo senza fornirgli input e restituisce stdout+stderr e
        l'exit code.
        """
        p = Popen(args, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
        out, err = p.communicate()
        if p.returncode != 0:
            print "ERROR executing:", " ".join(args)
        return out, p.returncode

    def _adaptInfos(self):
        """
        ritorna una copia del dizionario con le informazioni di deploy
        adattate alla distribuzione sotto linux
        """
        # È richiesto di poter avere più versioni dello stesso software
        # installate in parallelo, ho deciso quindi di aggiungere al nome del
        # file la versione senza il revision number. In questo modo ho che per
        # il gestore di pacchetti "Caligola 4.12" e "Caligola 4.12.1" sono lo
        # stesso software ed il secondo aggiorna il primo mentre "Caligola 4.11"
        # e "Caligola 4.12" sono due software diversi e posso tenerli installati
        # entrambi.
        infos = self.cfg['infos'].copy()

        if len(infos['base_version'].split('.')) > 2:
            grand_version = '.'.join(infos['base_version'].split('.')[:2])
        else:
            grand_version = infos['base_version']

        infos['base_name'] = cfg["name"]
        infos['pkg_name'] = '%s_%s' % (cfg["name"], grand_version.replace('-', '_'))

        if infos.get('install_multiple_versions', True):
            infos["name"] = infos['pkg_name']
        else:
            infos["name"] = infos['base_name']

        if 'summary' not in infos:
            infos['summary'] = cfg['name']
        if 'description' not in infos:
            infos['description'] = ''
        if 'license' not in infos:
            infos['license'] = 'Proprietary'
        if 'maintainer' not in infos:
            infos['maintainer'] = ''
        infos['release'] = 1

        if architecture()[0] == '32bit':
            infos['arch'] = 'i386'
        else:
            infos['arch'] = 'amd64'

        if 'application_group' not in infos:
            infos['application_group'] = 'Group: Applications/Text'
        return infos

    def prepare(self):
        """
        analizza la configurazione passata e crea il dizionario l'elenco dei
        file da includere nella distribuzione.
        """
        self._addExesToDistrib()
        self._addDesktopFilesToDistrib()
        self._fixDistribDestinationPath()

    def build(self):
        """
        da implementare in una classe derivata.

        entry point principale per la classe _LinuxDeploy, deve ritornare un
        fullpath al package appena creato.
        """
        raise NotImplementedError()

    def _commonContextForTemplates(self):
        return {
            'target_dir': self.targetDir(),
        }

    def preparePostInstallScript(self, script, ctx=None):
        d = self._commonContextForTemplates()
        if ctx:
            d.update(ctx)
        return template_str(script, d, str)

    def _fixDistribDestinationPath(self):
        """
        rende assoluti i path di destinazione nel dizionario distrib
        """
        distrib = self.distrib
        root = self.targetDir()
        for fpath, dst in distrib.items():
            if isinstance(dst, tuple):
                dst, template = dst
            else:
                template = False
            if dst.startswith('/') or (len(dst) > 2 and dst[1] == ':'):
                continue
            else:
                if not dst:
                    dpath = join(root, basename(fpath))
                else:
                    dpath = join(root, dst)
                distrib[fpath] = (dpath, template)

    def _addExesToDistrib(self):
        distrib = self.distrib
        for e in self.cfg['executables']:
            distrib[join(self.temp_dir, e['name'])] = e['name']
            if e['icons']:
                distrib[e['icons'][0]] = basename(e['icons'][0])

    def _addDesktopFilesToDistrib(self):
        # preparo un file .desktop minimale per ogni eseguibile

        distrib = self.distrib
        infos = self.infos
        dtemplate = join(self.deploy_dir, 'entry.desktop')
        for e in self.cfg['executables']:
            if not e.has_key('icons') or not e['icons']:
                continue
            dpath = join(self.temp_dir, '%s-%s.desktop' % (e['name'].lower(), infos['version']))
            shutil.copy2(dtemplate, dpath)

            ctx = dict(e)
            ctx['name'] = infos['name'].replace('_', ' ', 1)
            subdir = '/opt/%s/' % infos['name']
            ctx['exec'] = join(subdir, e['name'])
            if 'categories' not in ctx:
                ctx['categories'] = ''
            if 'icons' not in ctx:
                ctx['icon'] = ''
            else:
                ctx['icon'] = join(subdir, basename(e['icons'][0]))
            template(dpath, ctx, conv=str)

            distrib[dpath] = '/usr/share/applications/%s' % basename(dpath)

class DEBDeploy(_LinuxDeploy):
    def __init__(self, *args, **kw):
        try:
            self._r(['dpkg', '--version'])
            self._r(['fakeroot'])
        except OSError:
            raise TypeError()
        else:
            super(DEBDeploy, self).__init__(*args, **kw)

    def _adaptInfos(self):
        i = super(DEBDeploy, self)._adaptInfos()
        i['name'] = i['name'].replace('_', '-').lower()
        i['version'] = i['version'].replace('_', '-')
        # Dalla versione di Ubuntu 11.04 vengono eseguiti una serie di test su
        # .deb prima di installarli (vedi package lintian); uno di questi
        # controlla la presenza del campo Installed-Size. Dalle debian policy
        #
        # http://www.debian.org/doc/debian-policy/ch-controlfields.html#s-f-Installed-Size
        #
        # si evince che questo campo è trattato solo come un hint; inserisco un
        # valore fisso ma verosimile (pigrizia)
        i['installed_size'] = 30000
        return i

    def build(self):
        self.prepare()

        infos = self.infos
        if not infos['maintainer']:
            infos['maintainer'] = 'nobody'

        if not infos['description']:
            infos['description'] = 'none'

        # l'albero di directory necessario a costruire un deb deve contenere
        # solo la directory DEBIAN oltre, ovviamente, a i file di programma.
        builder_root = join(self.temp_dir, 'dotdeb')
        if isdir(builder_root):
            shutil.rmtree(builder_root)
        DEBIAN = join(builder_root, 'DEBIAN')
        os.makedirs(DEBIAN)

        control = join(DEBIAN, "control")
        shutil.copy2(join(self.deploy_dir, "control.in"), control)
        template(control, infos, conv=str)

        post = join(DEBIAN, "postinst")
        script = [ 'touch /opt/.placeholder' ] + self.cfg['post_install']
        with file(post, 'w') as f:
            l = [ '#!/bin/sh' ] + script
            f.write(self.preparePostInstallScript('\n'.join(l)))
        os.chmod(post, 0755)

        distrib = self.distrib
        for src, dst in distrib.items():
            if isinstance(dst, tuple):
                dst, tpl = dst
            else:
                tpl = False
            dst = join(builder_root, dst[1:])
            if not tpl:
                _copy_posix(src, dst)
            else:
                d = self._commonContextForTemplates()
                data = template_str(file(src).read(), d, str)
                file(dst, 'w').write(data)

        out, rcode = self._r(['fakeroot', 'dpkg', '-b', builder_root, self.temp_dir])
        if rcode == 0:
            dname = '%s_%s_%s.deb' % (infos['name'].lower(), infos['version'], infos['arch'])
            return join(self.temp_dir, dname)
        else:
            raise ValueError(rcode, out)

class RPMDeploy(_LinuxDeploy):
    def __init__(self, *args, **kw):
        try:
            self._r(['rpmbuild'])
        except OSError:
            raise TypeError()
        else:
            super(RPMDeploy, self).__init__(*args, **kw)

    def _adaptInfos(self):
        i = super(RPMDeploy, self)._adaptInfos()
        i['version'] = i['version'].replace('-', '_')
        return i

    def build(self):
        # la generazione del .rpm è demandata a rpmbuild, questa funzione deve
        # solo preparagli il terreno.
        #
        # 1. prepare un file .tar.gz contenente l'eseguibile di Caligola, le sue
        # dipendenze (file di supporto) e un file .spec che indichi a rpmbuild come
        # costruire il file .rpm
        #
        # 2. costruire un tree di directory con dei nomi prestabiliti che rpmbuild
        # possa utilizzare per i suoi scopi
        #
        # 3. lanciare rpmbuild
        self.prepare()

        target = self.targetDir()
        distrib = self.distrib
        infos = self.infos

        file_list = []
        infos['target_dir'] = target
        infos['description'] = '%description' + infos['description']
        for fpath, dpath in distrib.items():
            if isinstance(dpath, tuple):
                dpath, tpl = dpath
            else:
                tpl = False
            if isdir(fpath):
                count = len(os.listdir(fpath))
                if count:
                    file_list.append('"%s/*"' % dpath)
                else:
                    file_list.append('%%dir "%s"' % dpath)
            else:
                if os.access(fpath, os.X_OK):
                    file_list.append('%%attr(0555,root,root) "%s"' % dpath)
                else:
                    file_list.append('"%s"' % dpath)

        infos['file_list'] = '\n'.join([ '%files', '%defattr(-,root,root,-)' ] + sorted(file_list))
        if self.cfg['post_install']:
            infos['post_install'] = '%post\n' + self.preparePostInstallScript('\n'.join(self.cfg['post_install']))
        else:
            infos['post_install'] = ''

        # dopo aver manipolati le informazioni di configurazione non rimane che
        # creare il file .spec (che può chiamarsi in qualsiasi modo)
        spec = join(self.temp_dir, "rpm.spec")
        shutil.copy2(join(self.deploy_dir, "rpm.spec"), spec)
        template(spec, infos, conv=str)

        # adesso preparo un archivio, {name}-{version}.tar.gz, con questo layout:
        # {name}-{version}/
        #                   {specfile}
        #                   {distrib files}
        arch_path = join(self.temp_dir, '%s-%s.tar.gz' % (infos['name'], infos['version']))
        aroot = '%s-%s' % (infos['name'], infos['version'])
        archive = tarfile.open(arch_path, 'w:gz')
        try:
            archive.add(spec, join(aroot, basename(spec)))

            for fpath, dst in distrib.items():
                if isinstance(dst, tuple):
                    dst, tpl = dst
                else:
                    tpl = False
                if not tpl:
                    archive.add(fpath, aroot + dst)
                else:
                    d = self._commonContextForTemplates()
                    data = template_str(file(fpath).read(), d, str)
                    f = tempfile.NamedTemporaryFile()
                    f.write(data)
                    f.flush()
                    archive.add(f.name, aroot + dst)
        finally:
            archive.close()

        # la temp_dir è completamente sotto il controllo del processo di deploy,
        # posso farci quello che voglio. Ci costruisco l'albero di directory
        # richiesto da rpmbuild
        builder_root = join(self.temp_dir, 'rpmbuild')
        if isdir(builder_root):
            shutil.rmtree(builder_root)
        for d in 'BUILD', 'RPMS', 'SOURCES', 'SPECS', 'SRPMS':
            os.makedirs(join(builder_root, d))

        # devo configurare rpmbuild per usare il nuovo albero di directory
        # (altrimenti il default è /usr/src/rpm e non ho i permessi per
        # scriverci).
        out, rcode = self._r(['rpmbuild', '--eval', '%define _topdir ' + builder_root, '-tb', arch_path])
        if rcode == 0:
            rpm_dir = join(builder_root, 'RPMS/')
            for root, subdir, files in os.walk(rpm_dir):
                rpms = [ f for f in files if splitext(f)[1] == '.rpm' ]
                if rpms:
                    return join(root, rpms[0])
            assert False, 'I cannot find my rpm'
        else:
            raise ValueError(rcode, out)

def buildDEB(output_dir):
    """
    Prova a costruire un deb, ritorna True in caso di successo, False altrimenti
    """
    try:
        deb = DEBDeploy(cfg, deploy_dir, temp_dir, src2_dir)
    except TypeError:
        trace('dpkg not found, skipping deb creation')
        return False
    else:
        trace('dpkg present, try to build the deb')

    try:
        debpath = deb.build()
    except ValueError, e:
        trace('deb build failed with exit code: %s' % e.args[0])
        print e.args[1]
        sys.exit(2)
    else:
        trace('deb build succeed, moving *.deb in ' + output_dir)
        try:
            os.remove(os.sep.join([output_dir, os.path.basename(debpath)]))
        except OSError:
            pass
        shutil.move(debpath, output_dir)
    return True

def buildRPM(output_dir):
    """
    Prova a costruire un rpm, ritorna True in caso di successo, False altrimenti
    """
    try:
        rpm = RPMDeploy(cfg, deploy_dir, temp_dir, src2_dir)
    except TypeError:
        trace('rpmbuild not found, skipping rpm creation')
        return False
    else:
        trace('rpmbuild present, try to build the rpm')

    try:
        rpmpath = rpm.build()
    except ValueError, e:
        trace('rpm build failed with exit code: %s' % e.args[0])
        print e.args[1]
        sys.exit(2)
    else:
        trace('rpm build succeed, moving *.rpm in ' + output_dir)
        try:
            os.remove(os.sep.join([output_dir, os.path.basename(rpmpath)]))
        except OSError:
            pass
        shutil.move(rpmpath, output_dir)
    return True

def parseDeployCfg(config):
    deploy_cfg = join(root_dir, config)
    cfg = parseConfig(deploy_cfg, ambient=globals())
    if 'dist' in cfg:
        print "ERROR: rename 'dist' into 'distrib' in deploy.cfg"
        sys.exit(2)
    if 'distrib' not in cfg:
        print "ERROR: 'distrib' not defined in deploy.cfg"
        sys.exit(2)
    if 'post_install' not in cfg:
        cfg['post_install'] = []
    if 'pre_package' not in cfg:
        cfg['pre_package'] = []
    if 'installer' not in cfg:
        cfg['installer'] = {}
    return cfg

def parseAdditionalCfg(cfg):
    """
    ricerca in tutte le sottodirectory dei .cfg aggiuntivi e li fonde con la
    configurazione attuale.
    """
    trace('Discovery')
    it = os.walk(root_dir)
    it.next()
    suitable = 'deploy.cfg', 'deploy-%s.cfg' % sys.platform, 'deploy-%s.cfg' % platform()
    ambient = { 'infos': cfg['infos'] }
    for root, subdirs, files in it:
        for f in suitable:
            f = join(root, f)
            if not isfile(f):
                continue
            additional = parseConfig(f, ambient=ambient)

            distrib = additional.get('distrib', {})
            # i path dei file sorgenti in distrib possono essere
            # di due tipi: relativi o assoluti.  i path relativi
            # fanno riferimenti a file figli di src_dir, questi
            # file verranno copiati in src2_dir prima di essere
            # processati.  i path assoluti fanno riferimnto a
            # tutti i file fuori da src_dir (ad esempio quelli
            # presenti in extras)
            if f.startswith(src_dir):
                d = root[len(src_dir):]
                if len(d) and d[0] == '/':
                    d = d[1:]
            else:
                d = root
            # cfg['distrib'] può essere None (= non si vuole un
            # installer), quindi non dobbiamo usarlo come dizionario
            # se non serve strettamente.
            if distrib:
                if cfg['distrib'] is None:
                    raise NotImplementedError("%s ha un distrib, ma deploy.cfg ha distrib=None" % f)
                cfg['distrib'].update(dict(
                    (join(d, k), v) for k,v in distrib.items()
                ))
            cfg['installer'].update(additional.get('installer', {}))
            cfg['post_install'].extend(additional.get('post_install', []))
            cfg['pre_package'].extend(additional.get('pre_package', []))

def run(output_dir, quick, config):
    if not quick:
        trace("Cleanup")
        try:
            if exists(temp_dir):
                shutil.rmtree(temp_dir)
        except (OSError, IOError):
            print "ERRORE: impossibile rimuovere directory temporanea:"
            print temp_dir
            print "Rimuovere la directory manualmente e rilanciare il deploy."
            sys.exit(2)
        os.mkdir(temp_dir)

    trace("Parsing config file")
    # tra il parsing del cfg principale e quello dei cfg accessori inframezzo
    # la generazione del build number, in questo modo eventuali .cfg che usano
    # l'informazione sulla versione corrennte hanno accesso al valore corretto.
    cfg.update(parseDeployCfg(config))
    genbuildnumber(quick)
    parseAdditionalCfg(cfg)
    if not cfg.get("executables"):
        fatal("ERRORE: non e' stato specificato alcun eseguibile da costruire.\n"
              '(Controlla il campo "executables" nel file di configurazione)')

    # Completa le configurazioni degli eseguibili con i valori di default
    for i in range(len(cfg["executables"])):
        c = default_exe_cfg.copy()
        c.update(cfg["executables"][i])
        cfg["executables"][i] = c

    mirror_tree(quick)
    build_modules(quick)

    for exe in cfg["executables"]:
        if "entrypoint" in exe:
            create_executable(exe)
        else:
            if sys.platform.startswith('darwin'):
                base_app = join(deploy_dir, cfg['name'] + '.app')
                shutil.copytree(join(src2_dir, exe["name"] + '.app'),
                                base_app)
            else:
                if sys.platform.startswith("win"):
                    exe_suffix = ".exe"
                else:
                    exe_suffix = ""

                shutil.copy2(join(src2_dir, exe["name"] + exe_suffix),
                             join(temp_dir, exe["name"] + exe_suffix))

    create_dist(output_dir)

def main(args):
    from optparse import OptionParser

    p = OptionParser(usage="%prog [options]")
    p.add_option("-o", "--output-dir", type="string", metavar="PATH",
                 default=os.getcwd(),
                 help="Directory where to generate the final output file (default=current directory)")
    p.add_option("-q", "--quick", action="store_true", default=False,
                 help="Try being quicker: avoid rebuilding everything from scratch. This is UNSAFE "
                      "because there could be bugs in the dependencies and something could not be "
                      "recompiled.")
    p.add_option("-m", "--mingw32", default=False, action="store_true",
                 help="use mingw32 (default: %default)")
    p.add_option("-c", "--config", type="string", default="deploy.cfg",
                 help="load deploy configuration from file (default=deploy.cfg)")
    global opts
    opts, args = p.parse_args(args)

    if sys.platform.startswith("darwin") and sys.prefix.startswith("/System"):
        fatal("System python unsupported; please install MacPython from http://www.python.org/", "!")

    if __debug__:
        fatal("Run 'python -O' for deploy!", "!")

    if args:
        p.error("too many positional arguments")

    run(opts.output_dir, opts.quick, opts.config)
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
