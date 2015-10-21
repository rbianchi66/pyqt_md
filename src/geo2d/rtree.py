from geo2d import P

#
# Every node will have up to LIMIT-1 elements
# LIMIT must be even > 2
#
LIMIT = 4

# Pre-computation of all possible splits
splits = []
for b in xrange(1<<(LIMIT-1)):
    split = [], []
    for j in xrange(LIMIT):
        split[(b >> j)&1].append(j)
    if len(split[0]) == len(split[1]):
        splits.append(split)

class Node(object):
    """
    Abstract node; contains a bounding box
    """
    def __init__(self):
        self.x0 = self.y0 = self.x1 = self.y1 = None

    def updateP(self, p):
        """
        Updates the bounding box given a new point
        """
        if self.x0 is None or self.x0 > p.x: self.x0 = p.x
        if self.y0 is None or self.y0 > p.y: self.y0 = p.y
        if self.x1 is None or self.x1 < p.x: self.x1 = p.x
        if self.y1 is None or self.y1 < p.y: self.y1 = p.y

    def updateBB(self, x0, y0, x1, y1):
        """
        Updates the bounding box considering the addition of a new rect
        """
        if self.x0 is None or self.x0 > x0: self.x0 = x0
        if self.y0 is None or self.y0 > y0: self.y0 = y0
        if self.x1 is None or self.x1 < x1: self.x1 = x1
        if self.y1 is None or self.y1 < y1: self.y1 = y1

    def area(self):
        """
        Area of the bounding box
        """
        return (self.x1 - self.x0) * (self.y1 - self.y0)

    def center(self):
        """
        Center of the bounding box
        """
        return P((self.x0 + self.x1)/2, (self.y0 + self.y1)/2)

    def dist(self, p):
        """
        Returns the distance between p and the bounding box of the node
        """
        if p.x < self.x0:
            x = self.x0
        elif p.x > self.x1:
            x = self.x1
        else:
            x = p.x
        if p.y < self.y0:
            y = self.y0
        elif p.y > self.y1:
            y = self.y1
        else:
            y = p.y
        return abs(p - P(x, y))


class Leaf(Node):
    """
    A leaf node in the R-Tree structure; it can contain
    up to LIMIT-1 points, where a point is a tuple (p, info)
    with p being a 2d point P and info being the opaque payload.
    """
    def __init__(self, pts):
        """
        Builds a leaf node given a list of (p, info) tuples
        """
        Node.__init__(self)
        self.pts = pts[:]
        for p, i in self.pts:
            self.updateP(p)

    def add(self, (p, info)):
        """
        Adds a new pair (p, info) to the node, returns None if
        the node can accomodate it, or returns a pair of two new
        leaves that must replace current leaf because of a split
        """
        self.pts.append((p, info))
        if len(self.pts) == LIMIT:
            best = None
            for L1, L2 in splits:
                n1 = Leaf([self.pts[j] for j in L1])
                n2 = Leaf([self.pts[j] for j in L2])
                a = n1.area() + n2.area()
                if best is None or best[0] > a:
                    best = a, n1, n2
            return best[1:]
        else:
            self.updateP(p)
            return None

    def closest(self, p, bsf=None):
        """
        Returns the (p, info) pair closest to p
        """
        return min((abs(e[0] - p), e) for e in self.pts)[1]

class MNode(Node):
    """
    A non-leaf node in the R-Tree; it can contain up to
    LIMIT-1 other nodes (either leaves or not).
    """
    def __init__(self, elements):
        """
        Builds a new node given the list of subnodes
        """
        Node.__init__(self)
        self.elements = elements[:]
        for e in self.elements:
            self.updateBB(e.x0, e.y0, e.x1, e.y1)

    def add(self, (p, info)):
        """
        Adds a new pair (p, info) to the subtree rooted at this
        node. Returns None if the subtree can accomodate it, or
        returns a pair of two new nodes that must replace current
        one because of a split
        """
        self.updateP(p)

        # Pick the element with the closest center
        n = min((abs(p - e.center()), e) for e in self.elements)[1]

        # Add the element to that subtree
        result = n.add((p, info))

        if result is not None:
            # Overflow occurred; replace the node with the two new nodes
            self.elements.remove(n)
            self.elements.extend(result)
            if len(self.elements) == LIMIT:
                # This node must split
                best = None
                for L1, L2 in splits:
                    n1 = MNode([self.elements[j] for j in L1])
                    n2 = MNode([self.elements[j] for j in L2])
                    a = n1.area() + n2.area()
                    if best is None or best[0] > a:
                        best = a, n1, n2
                return best[1:]
            else:
                return None
        else:
            return None

    def closest(self, p, bsf=None):
        """
        Returns the (p, info) pair closest to p in the subtree rooted
        at this node; avoid searching for results that are known to be
        farther than bsf.
        It may return None if nothing closer than bsf exists.
        """
        C = [(e.dist(p), e) for e in self.elements]
        C.sort()
        best = None
        for d, e in C:
            if bsf is not None and d > bsf:
                break
            te = e.closest(p, bsf)
            if te is not None:
                td = abs(te[0] - p)
                if bsf is None or td < bsf:
                    bsf = td
                if best is None or best[0] > td:
                    best = td, te
        if best is None:
            return None
        else:
            return best[1]

class RTree2D(object):
    """
    An R-tree data structure; it contains (point, info) pairs
    and can be queried for what is the closest point to a
    given point in Log(n) time.
    """
    def __init__(self):
        self.root = None
        self.count = 0
        self.levels = 0

    def add(self, (p, info)):
        """
        Adds a (p, info) pair to the tree
        """
        if self.root is None:
            self.root = Leaf([(p, info)])
            self.count = 1
            self.levels = 1
        else:
            self.count += 1
            result = self.root.add((p, info))
            if result is not None:
                self.root = MNode(list(result))
                self.levels += 1

    def closest(self, p, bsf=None):
        """
        Returns the (p, info) pair closest to p or None if the tree is empty
        or if no data is closer than bsf
        """
        if self.root is None:
            return None
        else:
            res = self.root.closest(p, bsf)
            if bsf is not None and res is not None and abs(res[0] - p) > bsf:
                return None
            else:
                return res


if __name__ == "__main__":
    t = RTree2D()
    import random
    tp = [P(random.random()*1000, random.random()*1000)
          for i in xrange(20)]
    cl = [None] * len(tp)
    for x in xrange(10000):
        if x % 1000 == 0:
            print x
        p = P(random.random()*1000, random.random()*1000)
        t.add((p, x))
        for i in xrange(len(tp)):
            if cl[i] is None or abs(cl[i][0] - tp[i]) > abs(p - tp[i]):
                cl[i] = p, x
    print t.count, t.levels
    import time
    ttime = 0.0
    for i in xrange(len(tp)):
        start = time.time()
        for x in xrange(100):
            res = t.closest(tp[i])
        stop = time.time()
        ttime += stop - start
        if res == cl[i]:
            print "OK", res, "d =", abs(res[0] - tp[i])
        else:
            print "fail:", res, cl[i]
    print "Average time = %0.3f ms" % (ttime / 100 / len(tp) * 1000)
