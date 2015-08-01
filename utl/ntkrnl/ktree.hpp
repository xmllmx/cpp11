#pragma once

#if defined(RTL_USE_AVL_TABLES)
#undef RTL_USE_AVL_TABLES
#endif

//
// Note:
//
// 1. Both node type and key type must be inherited from ITreeNode;
// 2. A node must be comparable with a key, and vice versa;
//
INTERFACE ITreeNode
{
    virtual ~ITreeNode() = default;
    virtual bool operator <(const ITreeNode& other) const = 0;
    virtual bool operator ==(const ITreeNode& other) const = 0;
    virtual int GetNodeType(const ITreeNode& other) const = 0;
};

template<class TreeTable>
RTL_GENERIC_COMPARE_RESULTS NTAPI CompareTreeElements(TreeTable*, PVOID elem_1, PVOID elem_2)
{
    const auto& a = *static_cast<ITreeNode*>(elem_1);
    const auto& b = *static_cast<ITreeNode*>(elem_2);
        
    if (a < b)
    {
        return GenericLessThan;
    }
    else if (a == b)
    {
        return GenericEqual;
    }
    else
    {
        return GenericGreaterThan;
    }
}

inline RTL_GENERIC_COMPARE_RESULTS NTAPI CompareAvlTreeElements(RTL_AVL_TABLE* table, PVOID elem_1, PVOID elem_2)
{
    return CompareTreeElements(table, elem_1, elem_2);
}

inline RTL_GENERIC_COMPARE_RESULTS NTAPI CompareSplayTreeElements(RTL_GENERIC_TABLE* table, PVOID elem_1, PVOID elem_2)
{
    return CompareTreeElements(table, elem_1, elem_2);
}

template<class ValueType, bool t_is_avl, class LockType = NullLock, ENABLE_IF(IsConvertible<ValueType, ITreeNode>::value)>
class Tree
{
public:
    DELETE_COPY(Tree);

    Tree(Tree&& other)
        : _table(other._table), _table(other._lock)
    {
        other._table = nullptr;
    }

    DEFINE_MOVE_ASSIGNER(Tree);
    void swap(Tree& other)
    {
        if (this != &other)
        {
            Swap(_table, other._table);
            Swap(_lock, other._lock);
        }
    }

    virtual ~Tree()
    {
        LOCK_EXCLUSIVELY(_lock);

        auto fn_clean = [this](PVOID entry)
        {
            this->Delete<ValueType>(*static_cast<ValueType*>(entry));
        };

        if (t_is_avl)
        {
            for (auto entry = RtlEnumerateGenericTableAvl(*_table, true); entry; entry = RtlEnumerateGenericTableAvl(*_table, true))
            {
                fn_clean(entry);
            }
        }
        else
        {
            for (auto entry = RtlEnumerateGenericTable(*_table, true); entry; entry = RtlEnumerateGenericTable(*_table, true))
            {
                fn_clean(entry);
            }
        }
    }

public:
    bool IsEmpty() const
    {
        LOCK_SHAREDLY(_lock);

        if (t_is_avl)
        {
            return RtlIsGenericTableEmptyAvl(*_table);
        }
        else
        {
            return RtlIsGenericTableEmpty(*_table);
        }
    }

    ValueType* Insert(const ValueType& elem)
    {
        PVOID   ret_internal_elem = 0;
        BOOLEAN is_inserted = FALSE;

        LOCK_EXCLUSIVELY(_lock);

        if (t_is_avl)
        {
            ret_internal_elem = RtlInsertElementGenericTableAvl(_table, &const_cast<ValueType&>(elem), sizeof(elem), &is_inserted);
        }
        else
        {
            ret_internal_elem = RtlInsertElementGenericTable(_table, &const_cast<ValueType&>(elem), sizeof(elem), &is_inserted);
        }

        if (is_inserted)
        {
            new (ret_internal_elem) ValueType(elem);
        }
        
        return static_cast<ValueType*>(ret_internal_elem);
    }

    ValueType* Insert(ValueType&& elem)
    {
        PVOID   ret_internal_elem = 0;
        BOOLEAN is_inserted = FALSE;

        LOCK_EXCLUSIVELY(_lock);

        if (t_is_avl)
        {
            ret_internal_elem = RtlInsertElementGenericTableAvl(*_table, &elem, sizeof(elem), &is_inserted);
        }
        else
        {
            ret_internal_elem = RtlInsertElementGenericTable(*_table, &elem, sizeof(elem), &is_inserted);
        }

        if (is_inserted)
        {
            new (ret_internal_elem) ValueType(Move(elem));
        }

        return static_cast<ValueType*>(ret_internal_elem);
    }

    template<class KeyType, ENABLE_IF(IsConvertible<KeyType, ITreeNode>::value)>
    ValueType* Lookup(const KeyType& key) const
    {
        PVOID ret_internal_elem = nullptr;

        LOCK_SHAREDLY(_lock);

        if (t_is_avl)
        {
            ret_internal_elem = RtlLookupElementGenericTableAvl(*_table, const_cast<KeyType*>(&key));
        }
        else
        {
            ret_internal_elem = RtlLookupElementGenericTable(*_table, const_cast<KeyType*>(&key));
        }

        return static_cast<ValueType*>(ret_internal_elem);
    }

    ValueType* Lookup(const ValueType& key) const
    {
        return this->Lookup<ValueType>(key);
    }

    template<class KeyType, ENABLE_IF(IsConvertible<KeyType, ITreeNode>::value)>
    bool Exists(const KeyType& key) const
    {
        return !!this->Lookup<KeyType>(key);
    }

    bool Exists(const ValueType& key) const
    {
        return !!this->Lookup(key);
    }

    template<class KeyType, ENABLE_IF(IsConvertible<KeyType, ITreeNode>::value)>
    bool Delete(const KeyType& key)
    {
        LOCK_EXCLUSIVELY(_lock);

        if (t_is_avl)
        {
            return !!RtlDeleteElementGenericTableAvl(*_table, const_cast<KeyType*>(&key));
        }
        else
        {
            return !!RtlDeleteElementGenericTable(*_table, const_cast<KeyType*>(&key));
        }
    }

    bool Delete(const ValueType& key)
    {
        return this->Delete<ValueType>(key);
    }

    size_t GetElementCount() const
    {
        LOCK_SHAREDLY(_lock);

        if (t_is_avl)
        {
            return RtlNumberGenericTableElementsAvl(*_table);
        }
        else
        {
            return RtlNumberGenericTableElements(*_table);
        }
    }

protected:
    Tree(PRTL_AVL_COMPARE_ROUTINE fn_comp)
        : _table(new _TreeTable)
    {
        RtlInitializeGenericTableAvl(*_table, fn_comp, Tree::_AllocateNode<RTL_AVL_TABLE>, Tree::_FreeNode<RTL_AVL_TABLE>, nullptr);
    }

    Tree(PRTL_GENERIC_COMPARE_ROUTINE fn_comp)
        : _table(new _TreeTable)
    {
        RtlInitializeGenericTable(*_table, fn_comp, Tree::_AllocateNode<RTL_GENERIC_TABLE>, Tree::_FreeNode<RTL_GENERIC_TABLE>, nullptr);
    }

protected:
    template<class TableType>
    static PVOID NTAPI _AllocateNode(TableType* table, CLONG node_size)
    {
        return operator new(node_size, PagedPool); // RtlXXXGenricTableXXX must be called at IRQL < DISPATCH_LEVEL, so PagedPool is enough.
    }

    template<class TableType>
    static VOID NTAPI _FreeNode(TableType* table, PVOID node)
    {
        Destroy(static_cast<ITreeNode*>(node));
        operator delete(node);
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

    _TreeTable*      _table;
    mutable LockType _lock;
};

template<class ValueType, class LockType = NullLock>
class AvlTree : public Tree<ValueType, true, LockType>
{
public:
    AvlTree()
        : Tree(CompareAvlTreeElements)
    {}
};

template<class ValueType, class LockType = NullLock>
class SplayTree : public Tree<ValueType, false, LockType>
{
public:
    SplayTree()
        : Tree(CompareSplayTreeElements)
    {}
};