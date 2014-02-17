#pragma once

#if defined(RTL_USE_AVL_TABLES)
#undef RTL_USE_AVL_TABLES
#endif

template<class ElementType, class TreeTable>
RTL_GENERIC_COMPARE_RESULTS NTAPI CompareTreeElements(TreeTable*, PVOID elem_1, PVOID elem_2)
{
    const auto& a = *static_cast<ElementType*>(elem_1);
    const auto& b = *static_cast<ElementType*>(elem_2);

    if (a == b)
    {
        return GenericEqual;
    }
    else if (a < b)
    {
        return GenericLessThan;
    }
    else
    {
        return GenericGreaterThan;
    }
}

template<class ElementType>
RTL_GENERIC_COMPARE_RESULTS NTAPI CompareAvlTreeElements(RTL_AVL_TABLE* tree_table, PVOID elem_1, PVOID elem_2)
{
    return CompareTreeElements<ElementType>(tree_table, elem_1, elem_2);
}

template<class ElementType>
RTL_GENERIC_COMPARE_RESULTS NTAPI CompareSplayTreeElements(RTL_GENERIC_TABLE* tree_table, PVOID elem_1, PVOID elem_2)
{
    return CompareTreeElements<ElementType>(tree_table, elem_1, elem_2);
}

template<class Element>
class Tree
{
public:
    DELETE_COPY(Tree);

protected:
    ~Tree()
    {
        Function<Element*()>     fn_get_first_elem;
        Function<void(Element*)> fn_delete_elem;

        if (_is_avl)
        {
            fn_get_first_elem = [this]()
            {
                return static_cast<Element*>(RtlGetElementGenericTableAvl(_table, 0));
            };

            fn_delete_elem = [this](Element* internal_elem)
            {
                RtlDeleteElementGenericTableAvl(_table, internal_elem);
            };
        }
        else
        {
            fn_get_first_elem = [this]()
            {
                return static_cast<Element*>(RtlGetElementGenericTable(_table, 0));
            };

            fn_delete_elem = [this](Element* internal_elem)
            {
                RtlDeleteElementGenericTable(_table, internal_elem);
            };
        }

        auto fn_clear = [this](Function<Element*()> fn_get_first_element, Function<void(Element*)> fn_delete_element)
        {
            while (auto internal_elem = fn_get_first_element())
            {
                Destruct(internal_elem);
                fn_delete_element(internal_elem);
            }
        };

        fn_clear(fn_get_first_elem, fn_delete_elem);

        _table = {};
        _is_npaged = {};
        _is_avl = {};
    }

    Tree(PRTL_AVL_COMPARE_ROUTINE fn_comp, bool is_npaged = true)
        : _table(), _is_npaged(is_npaged), _is_avl(true), _cur_pos()
    {
        Assert(fn_comp);
        RtlInitializeGenericTableAvl(_table, fn_comp, Tree::_AllocateNode<RTL_AVL_TABLE>, Tree::_FreeNode<RTL_AVL_TABLE>, 0);
    }

    Tree(PRTL_GENERIC_COMPARE_ROUTINE fn_comp, bool is_npaged = true)
        : _table(), _allocator(), _is_npaged(is_npaged), _is_avl(false), _cur_pos()
    {
        Assert(fn_comp);
        RtlInitializeGenericTable(_table, fn_comp, Tree::AllocateNode<RTL_GENERIC_TABLE>, Tree::FreeNode<RTL_GENERIC_TABLE>, 0);
    }

public:
    Element* Insert(const Element& elem)
    {
        PVOID   ret_internal_elem = 0;
        BOOLEAN is_inserted = FALSE;

        if (_is_avl)
        {
            ret_internal_elem = RtlInsertElementGenericTableAvl(_table, &const_cast<Element&>(elem), sizeof(elem), &is_inserted);
        }
        else
        {
            ret_internal_elem = RtlInsertElementGenericTable(_table, &const_cast<Element&>(elem), sizeof(elem), &is_inserted);
        }

        if (!is_inserted)
        {
            ret_internal_elem = 0;
        }

        if (ret_internal_elem)
        {
            new (ret_internal_elem) Element(elem);
        }

        return static_cast<Element*>(ret_internal_elem);
    }

    Element* Lookup(const Element& elem)
    {
        PVOID ret_internal_elem = 0;

        if (_is_avl)
        {
            ret_internal_elem = RtlLookupElementGenericTableAvl(_table, &const_cast<Element&>(elem));
        }
        else
        {
            ret_internal_elem = RtlLookupElementGenericTable(_table, &const_cast<Element&>(elem));
        }

        return static_cast<Element*>(ret_internal_elem);
    }

    bool Delete(const Element& elem)
    {
        if (auto entry = this->Lookup(elem))
        {
            DEFER(Destruct(entry));
        }
        else
        {
            return false;
        }

        bool is_success = false;

        if (_is_avl)
        {
            is_success = !!RtlDeleteElementGenericTableAvl(_table, &const_cast<Element&>(elem));
        }
        else
        {
            is_success = !!RtlDeleteElementGenericTable(_table, &const_cast<Element&>(elem));
        }

        Assert(is_success);

        return is_success;
    }

    size_t GetElementCount() const
    {
        if (_is_avl)
        {
            return RtlNumberGenericTableElementsAvl(_table);
        }
        else
        {
            return RtlNumberGenericTableElements(_table);
        }
    }

    void PrepareToIterate() const
    {
        _cur_pos = nullptr;
    }

    //
    // If prev_internal_elem is 0, then GetNext returns the first entry
    //
    Element* GetNext()
    {
        if (_is_avl)
        {
            return RtlEnumerateGenericTableWithoutSplayingAvl(_table, &_cur_pos);
        }
        else
        {
            return RtlEnumerateGenericTableWithoutSplaying(_table, &_cur_pos);
        }
    }

    UniquePtr<Element[]> Copy()
    {
        auto fn_copy_assign = [this](Element& external_elem, void* internal_elem)
        {
            external_elem = *static_cast<Element*>(internal_elem);
        };

        return this->_Fetch(fn_copy_assign);
    }

    UniquePtr<Element[]> Move()
    {
        DEFER(Clear());

        auto fn_move_assign = [this](Element& external_elem, void* internal_elem)
        {
            external_elem = Move(*static_cast<Element*>(internal_elem));
        };

        return this->_Fetch(fn_move_assign);
    }

protected:
    template<class TableType>
    static PVOID NTAPI _AllocateNode(TableType* table, CLONG node_size)
    {
        Assert(table && node_size);

        return Allocate(node_size);
    }

    template<class TableType>
    static VOID NTAPI _FreeNode(TableType* table, PVOID node)
    {
        Assert(table && node);

        Free(node);
    }

    UniquePtr<Element[]> _Fetch(Function<void(Element&, void* internal_elem)> fn_assign)
    {
        auto elem_count = this->GetElementCount();
        UniquePtr<Element[]> coll(elem_count);

        coll.set_valid_size(coll.get_max_size());

        PVOID prev_internal_elem = 0;
        PVOID cur_internal_elem = 0;

        FOR(idx, elem_count)
        {
            if (_is_avl)
            {
                cur_internal_elem = RtlEnumerateGenericTableWithoutSplayingAvl(_table, &prev_internal_elem);
            }
            else
            {
                cur_internal_elem = RtlEnumerateGenericTableWithoutSplaying(_table, &prev_internal_elem);
            }

            Assert(cur_internal_elem);

            fn_assign(coll[idx], cur_internal_elem);
        }

        return coll;
    }

private:
    union _TreeTable
    {
        operator PRTL_GENERIC_TABLE() const
        {
            return const_cast<PRTL_GENERIC_TABLE>(&_generic_table);
        }

        operator PRTL_AVL_TABLE() const
        {
            return const_cast<PRTL_AVL_TABLE>(&_avl_table);
        }

    private:
        RTL_GENERIC_TABLE _generic_table;
        RTL_AVL_TABLE     _avl_table;
    };

    _TreeTable    _table;
    bool          _is_npaged;
    bool          _is_avl;
    mutable PVOID _cur_pos;
};

template<class Element>
class AvlTree : public Tree<Element>
{
public:
    AvlTree(PRTL_AVL_COMPARE_ROUTINE fn_comp, bool is_npaged = true)
        : Tree(fn_comp, is_npaged)
    {}
};

template<class Element>
class SplayTree : public Tree<Element>
{
public:
    SplayTree(PRTL_GENERIC_COMPARE_ROUTINE fn_comp, bool is_npaged = true)
        : Tree(fn_comp, is_npaged)
    {}
};

template<class Element, class Lock = EResource>
class SafeTree : protected Tree<Element>
{
    static_assert(Lock::is_reentrant, "The Lock class must be reentrant");

public:
    using Tree::PrepareToIterate;
    using Tree::GetElementCount;

    SafeTree(PRTL_AVL_COMPARE_ROUTINE fn_comp, bool is_npaged = true)
        : Tree(fn_comp, is_npaged)
    {}

    SafeTree(PRTL_GENERIC_COMPARE_ROUTINE fn_comp, bool is_npaged = true)
        : Tree(fn_comp, is_npaged)
    {}

    Element* Insert(const Element& elem)
    {
        LOCK_EXCLUSIVELY(_lock);
        return this->Tree::Insert(elem);
    }

    Element* Lookup(const Element& elem)
    {
        LOCK_SHAREDLY(_lock);
        return this->Tree::Lookup(elem);
    }

    bool Delete(const Element& elem)
    {
        LOCK_EXCLUSIVELY(_lock);
        return this->Tree::Delete(elem);
    }

    Element* GetNext()
    {
        LOCK_SHAREDLY(_lock);
        return this->Tree::GetNext();
    }

    UniquePtr<Element[]> Copy()
    {
        LOCK_SHAREDLY(_lock);
        return this->Tree::Copy();
    }

    UniquePtr<Element[]> Move()
    {
        LOCK_EXCLUSIVELY(_lock);
        return this->Tree::Move();
    }

    Lock& GetLock()
    {
        return _lock;
    }

private:
    Lock _lock;
};