#if !defined(IMAGE2D_H_INCLUDED)
#define IMAGE2D_H_INCLUDED

#include <vector>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <math.h>

template<typename Pixel>
struct Image
{
    int w, h;
    std::vector<Pixel> data;

    Image(int w, int h) : w(w), h(h), data(w*h)
    {
    }

    Pixel pixel(int x, int y) const
    {
        return data[y*w + x];
    }

    Pixel pixel(int x, int y, Pixel out) const
    {
        if (x >= 0 && x < w && y >= 0 && y < h)
            return data[y*w + x];
        else
            return out;
    }

    void setPixel(int x, int y, Pixel color)
    {
        if (x >= 0 && x <w && y >= 0 && y < h)
            data[y*w+x] = color;
    }

    void fastSetPixel(int x, int y, Pixel color)
    {
        data[y*w+x] = color;
    }

    void fastMaskSetSpan(int x0, int x1, int y, Pixel mask, Pixel color)
    {
        Pixel *wp = &data[y*w];
        while (x0 < x1)
        {
            wp[x0] = (wp[x0] & mask) | color;
            x0++;
        }
    }

    void maskSetSpan(int x0, int x1, int y, Pixel mask, Pixel color)
    {
        if (y >= 0 && y < h)
            fastMaskSetSpan(x0 < 0 ? 0 : x0,
                        x1 > w ? w : x1,
                        y,
                        mask,
                        color);
    }

    void fastSetSpan(int x0, int x1, int y, Pixel color)
    {
        Pixel *wp = &data[y*w];
        while (x0 < x1)
            wp[x0++] = color;
    }

    void setSpan(int x0, int x1, int y, Pixel color)
    {
        if (y >= 0 && y < h)
            fastSetSpan(x0 < 0 ? 0 : x0,
                        x1 > w ? w : x1,
                        y,
                        color);
    }

    void fastBox(int x0, int y0, int x1, int y1, Pixel color)
    {
        Pixel *wp = &data[y0*w];
        while (y0++ < y1)
        {
            for (int x=x0; x<x1; x++)
                wp[x] = color;
            wp += w;
        }
    }

    void box(int x0, int y0, int x1, int y1, Pixel color)
    {
        fastBox(x0 < 0 ? 0 : x0,
                x1 > w ? w : x1,
                y0 < 0 ? 0 : y0,
                y1 > h ? h : y1,
                color);
    }

    void xorColor(Pixel color)
    {
        for (int i=0; i<w*h; i++)
            data[i] ^= color;
    }

    void drawImage(const Image<Pixel>& other, int x, int y)
    {
        int xa = x;
        int xb = x + other.w;
        int ya = y;
        int yb = y + other.h;
        if (xa < 0) xa = 0;
        if (xb > w) xb = w;
        if (ya < 0) ya = 0;
        if (yb > h) yb = h;
        while (ya < yb)
        {
            Pixel *wp = &data[ya*w];
            for (int ix = xa; ix<xb; ix++)
                wp[ix] = other.pixel(ix-x, ya-y);
            ya++;
        }
    }
    void andImage(const Image<Pixel>& other, int x = 0, int y = 0)
    {
        int xa = x;
        int xb = x + other.w;
        int ya = y;
        int yb = y + other.h;
        if (xa < 0) xa = 0;
        if (xb > w) xb = w;
        if (ya < 0) ya = 0;
        if (yb > h) yb = h;
        while (ya < yb)
        {
            Pixel *wp = &data[ya*w];
            for (int ix = xa; ix<xb; ix++)
                wp[ix] = wp[ix] && other.pixel(ix-x, ya-y);
            ya++;
        }
    }
    void orImage(const Image<Pixel>& other, int x = 0, int y = 0)
    {
        int xa = x;
        int xb = x + other.w;
        int ya = y;
        int yb = y + other.h;
        if (xa < 0) xa = 0;
        if (xb > w) xb = w;
        if (ya < 0) ya = 0;
        if (yb > h) yb = h;
        while (ya < yb)
        {
            Pixel *wp = &data[ya*w];
            for (int ix = xa; ix<xb; ix++)
                wp[ix] = wp[ix] || other.pixel(ix-x, ya-y);
            ya++;
        }
    }

    static unsigned char pixel2pgm(Pixel x) { return (unsigned char) x; }
    static unsigned int pixel2ppm(Pixel x) { return (unsigned int) x; }
    static Pixel pgm2pixel(unsigned char x) { return (Pixel) x; }
    static Pixel ppm2pixel(unsigned int x) { return (Pixel) x; }

    /**
     * Salva una immagine in format PGM binario e torna true se tutto va bene.
     * Il parametro opzionale conv deve fornire una conversione da pixel
     * a unsigned char (default casting).
     */
    template<typename Conv>
    bool savePGM(const std::string& filename, const Conv& conv) const {
        FILE *f = fopen(filename.c_str(), "wb");
        if (f) {
            std::vector<unsigned char> row(w);
            fprintf(f, "P5\n%i %i 255\n", w, h);
            for(int y=0; y<h; y++) {
                for (int x=0; x<w; x++) {
                    row[x] = conv(data[y*w+x]);
                }
                if (int(fwrite(&row[0], 1, w, f)) != w) {
                    fclose(f);
                    return false;
                }
            }
            return fclose(f) == 0;
        } else {
            return false;
        }
    }
    bool savePGM(const std::string& filename) const {
        return savePGM(filename, pixel2pgm);
    }

    /**
     * Carica una immagine in formato PGM binario.
     * Il parametro opzionale conv deve fornire una conversione da unsigned
     * char a pixel (default casting).
     */
    template<typename Conv>
    static Image<Pixel> *loadPGM(const std::string& filename, const Conv& conv) {
        FILE *f = fopen(filename.c_str(), "rb");
        int c;
        if (!f) return NULL;
        if (fgetc(f) != 'P' || fgetc(f) != '5') { fclose(f); return NULL; }
        while ((c = fgetc(f)) != EOF && isspace(c)) ;
        while (c == '#') {
            while ((c = fgetc(f)) != EOF && c != '\n') ; // Skip over comments
            c = fgetc(f);
        }
        if (c == EOF) { fclose(f); return NULL; }
        ungetc(c, f);
        int w, h;
        if (fscanf(f, "%i %i %*i%*c", &w, &h) != 2 || w < 0 || h < 0) {
            fclose(f); return NULL;
        }
        Image<Pixel> *img = new Image<Pixel>(w, h);
        for (int i=0; i<w*h; i++) {
            if ((c = fgetc(f)) == EOF) { delete img; fclose(f); return 0; }
            img->data[i] = conv(c);
        }
        fclose(f);
        return img;
    }
    static Image<Pixel> *loadPGM(const std::string& filename) {
        return loadPGM(filename, pgm2pixel);
    }

    /**
     * Salva una immagine in format PPM binario e torna true se tutto va bene.
     * Il parametro opzionale conv deve fornire una conversione da pixel
     * a unsigned int 0x00RRGGBB (default casting).
     */
    template<typename Conv>
    bool savePPM(const std::string& filename, const Conv& conv) const {
        FILE *f = fopen(filename.c_str(), "wb");
        if (f) {
            std::vector<unsigned char> row(w*3);
            fprintf(f, "P6\n%i %i 255\n", w, h);
            for(int y=0; y<h; y++) {
                for (int x=0; x<w; x++) {
                    unsigned int v = conv(data[y*w+x]);
                    row[x*3] = (v>>16)&255;
                    row[x*3+1] = (v>>8)&255;
                    row[x*3+2] = v & 255;
                }
                if (int(fwrite(&row[0], 1, w, f)) != w) {
                    fclose(f);
                    return false;
                }
            }
            return fclose(f) == 0;
        } else {
            return false;
        }
    }
    bool savePPM(const std::string& filename) const {
        return savePPM(filename, pixel2ppm);
    }

    /**
     * Carica una immagine in formato PPM binario.
     * Il parametro opzionale conv deve fornire una conversione da unsigned
     * int (0x00RRGGBB) a pixel (default casting).
     */
    template<typename Conv>
    static Image<Pixel> *loadPPM(const std::string& filename, const Conv& conv) {
        FILE *f = fopen(filename.c_str(), "rb");
        int c;
        if (!f) return NULL;
        if (fgetc(f) != 'P' || fgetc(f) != '6') { fclose(f); return NULL; }
        while ((c = fgetc(f)) != EOF && isspace(c)) ;
        while (c == '#') {
            while ((c = fgetc(f)) != EOF && c != '\n') ; // Skip over comments
            c = fgetc(f);
        }
        if (c == EOF) { fclose(f); return NULL; }
        ungetc(c, f);
        int w, h;
        if (fscanf(f, "%i %i %*i%*c", &w, &h) != 2 || w < 0 || h < 0) {
            fclose(f); return NULL;
        }
        Image<Pixel> *img = new Image<Pixel>(w, h);
        for (int i=0; i<w*h; i++) {
            int r = fgetc(f);
            int g = fgetc(f);
            int b = fgetc(f);
            if (b == EOF) { delete img; fclose(f); return 0; }
            img->data[i] = conv(b + (r<<8) + (g<<16));
        }
        fclose(f);
        return img;
    }
    static Image<Pixel> *loadPPM(const std::string& filename) {
        return loadPPM(filename, ppm2pixel);
    }

    /**
     * Funzioni di test di utilita' x dilate
     */
    static bool testNZero(Pixel p) { return p; }
    static bool testZero(Pixel p) { return !p; }

    /**
     * "Allarga" le parti specificate dalla funzione di test del raggio in pixel indicato,
     * chiamando la funzione di output con x0, x1 e y per ogni span del risultato.
     */
    template<typename Test, typename Output>
    Output cbDilate(int r, Output output, Test test) {
        std::vector<int> dx;
        for (int y=-r; y<=r; y++) {
            dx.push_back(int(sqrt((r+0.25)*(r+0.25) - y*y)));
        }
        int h2 = dx.size();
        std::vector< int > data2(h2*(w+1));
        for (int y=0; y<h; y++) {
            Pixel *rp = &data[y*w];
            for (int x0=0; x0<w;) {
                while (x0<w && !test(rp[x0])) x0++;
                if (x0 < w) {
                    int x1 = x0+1;
                    while (x1 < w && test(rp[x1])) x1++;
                    for (int i=0; i<h2; i++) {
                        int yy = y - r + i;
                        if (yy >= 0 && yy < h) {
                            int *wp = &data2[(yy % h2)*(w + 1)];
                            int xa = x0 - dx[i], xb = x1 + dx[i];
                            if (xa < 0) xa = 0; if (xb > w) xb = w;
                            wp[xa] += 1; wp[xb] -= 1;
                        }
                    }
                    x0 = x1;
                }
            }
            if (y >= r) {
                int *rp = &data2[((y-r) % h2)*(w+1)];
                int lx = 0;
                for (int c=0,x=0; x<=w; x++) {
                    int oc = c; c += rp[x];
                    if (!oc && c) lx = x; else if (oc && !c) output(lx, x, y-r);
                    rp[x] = 0;
                }
            }
        }
        for (int y=std::max(h-r, 0); y<h; y++) {
            int *rp = &data2[(y % h2)*(w+1)];
            int lx = 0;
            for (int c=0,x=0; x<=w; x++) {
                int oc = c; c += rp[x];
                if (!oc && c) lx = x; else if (oc && !c) output(lx, x, y);
            }
        }
        return output;
    }

    template<typename Output>
    Output cbDilate(int r, Output output) {
        cbDilage(r, output, testNZero);
    }

    /**
     * "Allarga" le parti specificate dalla funzione di test del raggio in pixel indicato,
     * riempendo il risultato con il colore `color`.
     */
    template<typename Test>
    void dilate(int r, Pixel color, Test test) {
        std::vector<int> dx;
        for (int y=-r; y<=r; y++) {
            dx.push_back(int(sqrt((r+0.25)*(r+0.25) - y*y)));
        }
        int h2 = dx.size();
        std::vector< int > data2(h2*(w+1));
        for (int y=0; y<h; y++) {
            Pixel *rp = &data[y*w];
            for (int x0=0; x0<w;) {
                while (x0<w && !test(rp[x0])) x0++;
                if (x0 < w) {
                    int x1 = x0+1;
                    while (x1 < w && test(rp[x1])) x1++;
                    for (int i=0; i<h2; i++) {
                        int yy = y - r + i;
                        if (yy >= 0 && yy < h) {
                            int *wp = &data2[(yy % h2)*(w + 1)];
                            int xa = x0 - dx[i], xb = x1 + dx[i];
                            if (xa < 0) xa = 0; if (xb > w) xb = w;
                            wp[xa] += 1; wp[xb] -= 1;
                        }
                    }
                    x0 = x1;
                }
            }
            if (y >= r) {
                int *rp = &data2[((y-r) % h2)*(w+1)];
                Pixel *wp = &data[(y-r)*w];
                for (int c=0,x=0; x<w; x++) {
                    if ((c += rp[x])) wp[x] = color;
                    rp[x] = 0;
                }
            }
        }
        for (int y=std::max(h-r, 0); y<h; y++) {
            int *rp = &data2[(y % h2)*(w+1)];
            Pixel *wp = &data[y*w];
            for (int c=0,x=0; x<w; x++) {
                if ((c += rp[x])) wp[x] = color;
            }
        }
    }

    void dilate(int r, Pixel color)
    {
        dilate(r, color, testNZero);
    }
};

template<typename Pixel> struct SumTableData;
template<> struct SumTableData<unsigned char> { typedef int type; };
template<> struct SumTableData<signed char> { typedef int type; };
template<> struct SumTableData<unsigned short> { typedef int type; };
template<> struct SumTableData<signed short> { typedef int type; };
template<> struct SumTableData<unsigned int> { typedef int type; };
template<> struct SumTableData<signed int> { typedef int type; };
template<> struct SumTableData<float> { typedef double type; };
template<> struct SumTableData<double> { typedef double type; };

// Binarizza una immagine impostando a 0/1 tutti i pixel in funzione
// del fatto che la somma dei pixel dell'immagine sorgente in un box
// di lato side (dispari se non si vuole una traslazione del risultato)
// centrato sul pixel risulti superiore ad una soglia assegnata.
// La funzione puo' lavorare anche inplace con srcimg = dstimg.
template<typename Pixel1, typename Pixel2>
void binarize(const Image<Pixel1>& srcimg,
              Image<Pixel2>& dstimg,
              int side, typename SumTableData<Pixel1>::type threshold)
{
    if ((srcimg.w != dstimg.w) || (srcimg.h != dstimg.h))
        throw std::runtime_error("Image size mismatch in binarize()");
    int w = srcimg.w;
    int h = srcimg.h;
    int hside = side >> 1;

    // Calcolo prima scanline sumtable
    std::vector<typename SumTableData<Pixel1>::type> st(w * h);
    st[0] = srcimg.data[0];
    for (int x=1; x<w; x++) {
        st[x] = st[x-1] + srcimg.data[x];
    }

    // Calcolo scanline successive
    for (int y=1; y<h; y++) {
        const Pixel1 *rp = &srcimg.data[y*w];
        typename SumTableData<Pixel1>::type *wp = &st[y*w];
        wp[0] = wp[-w] + rp[0];
        for (int x=1; x<w; x++) {
            wp[x] = wp[x-1] + wp[x-w] - wp[x-w-1] + rp[x];
        }
    }

    // Calcolo risultato
    for (int y=0; y<h; y++) {
        int y0 = y - hside - 1; if (y0 < 0) y0 = 0;
        int y1 = y + side - hside - 1; if (y1 > h-1) y1 = h - 1;
        for (int x=0; x<w; x++) {
            int x0 = x - hside - 1; if (x0 < 0) x0 = 0;
            int x1 = x + side - hside - 1; if (x1 > w-1) x1 = w - 1;
            dstimg.data[y*w+x] = (threshold < (+ st[y0*w+x0] - st[y0*w+x1]
                                               - st[y1*w+x0] + st[y1*w+x1]));
        }
    }
}

// Esegue il flood_fill nell'immagine <img> a partire dal
// punto (<x>,<y>) e usando per il riempimento il colore <color>
template<typename Pixel>
void flood_fill(int x, int y, Pixel color, Image<Pixel> &image) {
    int w = image.w;
    int h = image.h;
    Pixel *img = &image.data[0];
    std::vector<int> tx, ty;
    tx.push_back(x);
    ty.push_back(y);

    int ta = 0;
    int to_fill = img[y*w+x];
    while (tx.size() > 0) {
        int xx = tx.back();
        int yy = ty.back();
        tx.pop_back();
        ty.pop_back();
        if (img[yy*w+xx] == to_fill) {
            while (xx > 0 && img[yy*w+xx-1] == to_fill)
                --xx;
            bool check_above = true;
            bool check_below = true;
            ta -= xx;

            if (xx > 0) {
                if (yy > 0 && img[(yy-1)*w+xx-1] == to_fill) {
                    if (check_above) {
                       check_above = false;
                       tx.push_back(xx-1);
                       ty.push_back(yy-1);
                    } else {
                        // Nop
                    }
                } else {
                    check_above = true;
                }
                if (yy < h-1 && img[(yy+1)*w+xx-1] == to_fill) {
                    if (check_below) {
                       check_below = false;
                       tx.push_back(xx-1);
                       ty.push_back(yy+1);
                    } else {
                        // Nop
                    }
                } else {
                    check_below = true;
                }
            }

            while (xx < w && img[yy*w+xx] == to_fill) {
                if (yy > 0) {
                    if (img[(yy-1)*w+xx] == to_fill) {
                        if (check_above) {
                           check_above = false;
                           tx.push_back(xx);
                           ty.push_back(yy-1);
                        } else {
                            // Nop
                        }
                    } else {
                        check_above = true;
                    }
                }
                if (yy < h-1) {
                    if (img[(yy+1)*w+xx] == to_fill) {
                        if (check_below) {
                           check_below = false;
                           tx.push_back(xx);
                           ty.push_back(yy+1);
                        } else {
                            // Nop
                        }
                    } else {
                        check_below = true;
                    }
                }
                img[yy*w+xx] = color;
                xx++;
            }

            if (xx < w-1) {
                if (yy > 0 && img[(yy-1)*w+xx+1] == to_fill) {
                    if (check_above) {
                       check_above = false;
                       tx.push_back(xx+1);
                       ty.push_back(yy-1);
                    } else {
                        // Nop
                    }
                } else {
                    check_above = true;
                }
                if (yy < h-1 && img[(yy+1)*w+xx+1] == to_fill) {
                    if (check_below) {
                       check_below = false;
                       tx.push_back(xx+1);
                       ty.push_back(yy+1);
                    } else {
                        // Nop
                    }
                } else {
                    check_below = true;
                }
            }

            ta += xx;
        }
    }
}
#endif
