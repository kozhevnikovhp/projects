using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Commands;

namespace ZZero.ZPlanner.Data.Entities
{
    enum ComparisonOperations { Equal, NotEqual, Less, Greater, LessOrEqual, GreaterOrEqual, IsEmpty }

    internal class ZList<T> : List<T>, INotifyCollectionChanged, IMovableList, IList
        where T : ZEntity
    {
        public event NotifyCollectionChangedEventHandler CollectionChanged;
        public event NotifyCollectionChangedEventHandler BeforeCollectionChanged;

        #region Properties
        public object Owner { get; set; }

        public new T this[int index]
        {
            get
            {
                return base[index];
            }
            set
            {
                if (index < 0 || index >= Count)
                    throw new IndexOutOfRangeException("The specified index is out of range.");
                var oldItem = base[index];
                OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, value, oldItem, index));
                base[index] = value;
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ReplaceListItemCommand<T>(this, index, oldItem, value);
                bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, value, oldItem, index));
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        object IList.this[int index]
        {
            get
            {
                return base[index];
            }
            set
            {
                if (index < 0 || index >= Count)
                    throw new IndexOutOfRangeException("The specified index is out of range.");
                var oldItem = base[index];
                OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, value, oldItem, index));
                base[index] = (T)value;
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ReplaceListItemCommand<T>(this, index, oldItem, (T)value);
                bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, value, oldItem, index));
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }
        #endregion 

        #region Methods
        internal void OnCollectionChanged(NotifyCollectionChangedEventArgs args)
        {
            var handler = CollectionChanged;
            if (handler != null)
                handler(this, args);
        }

        internal void OnBeforeCollectionChanged(NotifyCollectionChangedEventArgs args)
        {
            var handler = BeforeCollectionChanged;
            if (handler != null)
                handler(this, args);
        }

        public new void AddRange(IEnumerable<T> collection)
        {
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, collection));
            base.AddRange(collection);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddRangeListItemCommand<T>(this, collection);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, collection));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public new void Add(T item)
        {
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, item));
            base.Add(item);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddListItemCommand<T>(this, item);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, item));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        int IList.Add(object value)
        {
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, value));
            base.Add((T)value);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new AddListItemCommand<T>(this, (T)value);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, value));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            return base.Count - 1;
        }

        public new void Insert(int index, T item)
        {
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, item, index));
            base.Insert(index, item);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new InsertListItemCommand<T>(this, index, item);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, item, index));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        void IList.Insert(int index, object value)
        {
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, value, index));
            base.Insert(index, (T)value);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new InsertListItemCommand<T>(this, index, (T)value);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, value, index));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public new void RemoveAt(int index)
        {
            if (index < 0 || index >= Count)
                throw new IndexOutOfRangeException("The specified index is out of range.");
            var oldItem = base[index];
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, oldItem, index));
            base.RemoveAt(index);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveListItemCommand<T>(this, index, oldItem);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, oldItem, index));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        void IList.RemoveAt(int index)
        {
            if (index < 0 || index >= Count)
                throw new IndexOutOfRangeException("The specified index is out of range.");
            var oldItem = base[index];
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, oldItem, index));
            base.RemoveAt(index);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveListItemCommand<T>(this, index, oldItem);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, oldItem, index));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public new bool Remove(T item)
        {
            int index = base.IndexOf(item);
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, item, index));
            var result = base.Remove(item);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveListItemCommand<T>(this, index, item);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, item, index));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            return result;
        }

        void IList.Remove(object value)
        {
            int index = base.IndexOf((T)value);
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, value, index));
            base.Remove((T)value);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveListItemCommand<T>(this, index, (T)value);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, value, index));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public bool RemoveRange(IEnumerable<T> collection)
        {
            var result = true;
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, collection));
            foreach(T item in collection)
            {
                result = base.Remove(item) && result;
            }

            if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveRangeListItemCommand<T>(this, collection);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, collection));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            return result;
        }

        public new void Clear()
        {
            T[] collection = base.ToArray();
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            base.Clear();
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveRangeListItemCommand<T>(this, collection);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        void IList.Clear()
        {
            T[] collection = base.ToArray();
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            base.Clear();
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new RemoveRangeListItemCommand<T>(this, collection);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public void MoveAt(T item, int index)
        {
            int oldIndex = base.IndexOf(item);
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, item, index, oldIndex));
            base.Remove(item);
            base.Insert(index, item);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new MoveListItemCommand<T>(this, item, oldIndex, index);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, item, index, oldIndex));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        void IMovableList.MoveAt(object item, int index)
        {
            int oldIndex = base.IndexOf((T)item);
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, item, index, oldIndex));
            base.Remove((T)item);
            base.Insert(index, (T)item);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new MoveListItemCommand<T>(this, (T)item, oldIndex, index);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, item, index, oldIndex));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public void MoveFrom(int index, int newIndex)
        {
            T item = base[index];
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, item, newIndex, index));
            base.Remove(item);
            base.Insert(newIndex, item);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new MoveListItemCommand<T>(this, item, index, newIndex);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, item, newIndex, index));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public void Swap(T item1, T item2)
        {
            int index1 = base.IndexOf(item1);
            int index2 = base.IndexOf(item2);
            OnBeforeCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, null, index1, index2));
            base.Remove(item1);
            base.Insert(index2, item1);
            base.Remove(item2);
            base.Insert(index1, item2);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new SwapListItemCommand<T>(this, item1, item2);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Move, null, index1, index2));
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }
        #endregion

        #region Initialization
        public ZList(object owner)
            :base()
        {
            Owner = owner;
        }

        public ZList(object owner, int capacity)
            :base(capacity)
        {
            Owner = owner;
        }

        public ZList(object owner, IEnumerable<T> collection)
            : base(collection)
        {
            Owner = owner;
        }
        #endregion

        public T GetByID(string id)
        {
            return this.Find(x => x.ID == id);
        }
    }

    public interface IMovableList : IList
    {
        void MoveAt(object item, int index);
    }
}
