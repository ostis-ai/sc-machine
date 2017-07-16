from sc import *

class ScSet:
    """Implements sc-set logic class.
    It allows to add/remove elements easier.
    """

    class Iterator:
        def __init__(self, it):
            self.iter = it

        def __next__(self):
            return self.next()

        def next(self):
            if self.iter.Next():
                return self.iter.Get(2)

            raise StopIteration

    def __init__(self, ctx, addr):
        self.ctx = ctx
        self.addr = addr

    def Has(self, elAddr):
        """Check if specified element exists in set
        """
        return self.ctx.HelperCheckEdge(self.addr, elAddr, ScType.EdgeAccessConstPosPerm)

    def Add(self, elAddr):
        """Add element into set. If element added,
        then returns True; otherwise - False
        """
        if not self.Has(elAddr):
            edge = self.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, self.addr, elAddr)
            return edge.IsValid()

        return False

    def Remove(self, elAddr):
        """Remove element from a set. If element removed,
        then returns True; otherwise - False
        """
        it = self.ctx.Iterator3(
            self.addr,
            ScType.EdgeAccessConstPosPerm,
            elAddr)
        
        if it.Next():
            self.ctx.DeleteElement(it.Get(1))
            return True

        return False

    def Clear(self):
        """Remove all elements from a set
        """
        it = self.ctx.Iterator3(
            self.addr,
            ScType.EdgeAccessConstPosPerm,
            ScType.Unknown)
        
        while it.Next():
            self.ctx.DeleteElement(it.Get(1))

    def __iter__(self):
        """Create iterator for iterate all elements of set
        Usage:
        for el in _set.IterElements():
            # process element
        """
        it = self.ctx.Iterator3(
            self.addr,
            ScType.EdgeAccessConstPosPerm,
            ScType.Unknown)

        return ScSet.Iterator(it)