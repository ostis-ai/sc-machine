from sc import *

class Iterator:
    def __init__(self, it):
        self.iter = it

    def __next__(self):
        return self.next()

    def next(self):
        if self.iter.Next():
            return self.iter.Get(2)

        raise StopIteration

class ScSet:
    """Implements sc-set logic class.
    It allows to add/remove elements easier.
    """
    def __init__(self, ctx, addr):
        self.ctx = ctx
        self.addr = addr

    def Has(self, elAddr: ScAddr) -> bool:
        """Check if specified element exists in set
        """
        return self.ctx.HelperCheckEdge(self.addr, elAddr, ScType.EdgeAccessConstPosPerm)

    def Add(self, elAddr: ScAddr) -> bool:
        """Add element into set. If element added,
        then returns True; otherwise - False
        """
        if not self.Has(elAddr):
            edge = self.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, self.addr, elAddr)
            return edge.IsValid()

        return False

    def Remove(self, elAddr: ScAddr) -> bool:
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

        return Iterator(it)


# ---------------------------------------
class ScRelationSet:
    """Set of elements that linekd with element by specified relation.
    This set class controls instances of:
    el1
        => nrel_relation: el2;
        => nrel_relation: el3;
        => nrel_relation: el4;;
    """
    

    def __init__(self, ctx, addr, relAddr):
        """
        addr - ScAddr of element
        relAddr - ScAddr of relation
        """
        self.ctx = ctx
        self.addr = addr
        self.relAddr = relAddr

    def Has(self, elAddr: ScAddr) -> bool:
        """Check if specified element exists in set
        """
        it = self.ctx.Iterator5(
            self.addr,
            ScType.EdgeDCommonConst,
            elAddr,
            ScType.EdgeAccessConstPosPerm,
            self.relAddr)

        return it.Next()

    def Add(self, elAddr: ScAddr) -> bool:
        """Add element into set. If element added,
        then returns True; otherwise - False
        """
        if not self.Has(elAddr):
            edge = self.ctx.CreateEdge(ScType.EdgeDCommonConst, self.addr, elAddr)
            edge = self.ctx.CreateEdge(ScType.EdgeAccessConstPosPerm, self.relAddr, edge)

            return edge.IsValid()

        return False

    def Remove(self, elAddr: ScAddr) -> bool:
        """Remove element from a set. If element removed,
        then returns True; otherwise - False
        """
        it = self.ctx.Iterator5(
            self.addr,
            ScType.EdgeDCommonConst,
            elAddr,
            ScType.EdgeAccessConstPosPerm,
            self.relAddr)
        
        if it.Next():
            self.ctx.DeleteElement(it.Get(1))
            return True

        return False

    def Clear(self):
        """Remove all elements from a set
        """
        it = self.ctx.Iterator5(
            self.addr,
            ScType.EdgeDCommonConst,
            ScType.Unknown,
            ScType.EdgeAccessConstPosPerm,
            self.relAddr)
        
        while it.Next():
            self.ctx.DeleteElement(it.Get(1))

    def __iter__(self):
        """Create iterator for iterate all elements of set
        Usage:
        for el in _set.IterElements():
            # process element
        """
        it = self.ctx.Iterator5(
            self.addr,
            ScType.EdgeDCommonConst,
            ScType.Unknown,
            ScType.EdgeAccessConstPosPerm,
            self.relAddr)

        return Iterator(it)