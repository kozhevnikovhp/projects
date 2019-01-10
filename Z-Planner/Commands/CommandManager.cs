using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZZero.ZPlanner.Commands
{
    /// <summary>
    /// Used for declaring <see cref="CommandManager.CommandsChanged"/> event.
    /// </summary>
    /// <param name="sender">Object that triggered the event.</param>
    /// <param name="e">Information about the event.</param>
    public delegate void CommandsEventHandler(object sender, CommandsEventArgs e);

    /// <summary>
    /// Provides data for the command events.
    /// </summary>
    public class CommandsEventArgs : EventArgs
    {
    }

    /// <summary>
    /// Provides Undo/Redo functionality for commands.
    /// </summary>
    class CommandManager
    {
        /// <summary>
        /// Event occurs when command is run.
        /// </summary>
        public event CommandsEventHandler CommandsChanged;

        /// <summary>
        /// List of Commands for Undo/Redo.
        /// </summary>
        private List<AbstractCommand> commandList;
        
        /// <summary>
        /// Last Undo command index.
        /// </summary>
        private int lastUndoIndex = -1;

        /// <summary>
        /// Maximum count of Undo/Redo commands.
        /// </summary>
        private uint commandsLimit = 0;

        /// <summary>
        /// Flag to ignore new commands during Undo/Redo execution.
        /// </summary>
        public bool IsIgnoreCommands { get; set; }

        /// <summary>
        /// Set flag to start keeping some operations as one command.
        /// </summary>
        public void StartComplexCommand()
        {
            IsIgnoreCommands = true;
        }

        /// <summary>
        /// Set flag to finish keeping some operations as one command.
        /// </summary>
        public void FinishComplexCommand()
        {
            IsIgnoreCommands = false;
        }

        /// <summary>
        /// Indicates whether the complex command flag was set.
        /// </summary>
        public bool IsComplexCommandStarted()
        {
            return IsIgnoreCommands;
        }

        /// <summary>
        /// Suspend rising command events.
        /// </summary>
        public bool SuspendCommandEvent()
        {
            bool oldValue = IsIgnoreCommands;
            IsIgnoreCommands = true;
            return oldValue;
        }

        /// <summary>
        /// Resume rising command events.
        /// </summary>
        /// <param name="newValue">Value of IsIgnoreCommands to be restored.</param>
        public void ResumeCommandEvent(bool newValue)
        {
            IsIgnoreCommands = newValue;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="CommandManager"/> class.
        /// </summary>
        /// <param name="limit">
        /// Maximum count of Undo/Redo commands.
        /// </param>
        internal CommandManager(uint limit)
        {
            this.commandList = new List<AbstractCommand>();
            this.commandsLimit = limit;
            this.IsIgnoreCommands = false;
        }

        /// <summary>
        /// Gets the count of available Undo commands.
        /// </summary>
        /// <value>Unsigned integer value.</value>
        internal uint UndoCount
        {
            get
            {
                return (uint)(this.lastUndoIndex + 1);
            }
        }

        /// <summary>
        /// Gets the count of available Redo commands.
        /// </summary>
        /// <value>Unsigned integer value.</value>
        internal uint RedoCount
        {
            get
            {
                return (uint)(this.commandList.Count - this.lastUndoIndex - 1);
            }
        }

        /// <summary>
        /// Clears the Undo/Redo command list.
        /// </summary>
        internal void Clear()
        {
            this.commandList.Clear();
            this.lastUndoIndex = -1;
            this.IsIgnoreCommands = false;
            this.OnCommandsChanged();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal void Undo()
        {
            if (this.lastUndoIndex < 0)
            {
                return;
            }

            AbstractCommand command = this.commandList[this.lastUndoIndex--];
            this.IsIgnoreCommands = true;
            command.Undo();
            this.IsIgnoreCommands = false;
            this.OnCommandsChanged();
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal void Redo()
        {
            if (this.lastUndoIndex + 1 >= this.commandList.Count)
            {
                return;
            }

            AbstractCommand command = this.commandList[this.lastUndoIndex++ + 1];
            this.IsIgnoreCommands = true;
            command.Redo();
            this.IsIgnoreCommands = false;
            this.OnCommandsChanged();
        }

        /// <summary>
        /// Occurs when the command list is changed.
        /// </summary>
        private void OnCommandsChanged()
        {
            if (null != this.CommandsChanged)
            {
                this.CommandsChanged(this, new CommandsEventArgs());
            }
        }

        /// <summary>
        /// Adds new command to the command list.
        /// </summary>
        /// <param name="AbstractCommand">A command to be added.</param>
        internal void Add(AbstractCommand command)
        {
            if (this.IsIgnoreCommands) return;

            if (this.lastUndoIndex + 1 < this.commandList.Count)
            {
                this.commandList.RemoveRange(this.lastUndoIndex + 1, this.commandList.Count - this.lastUndoIndex - 1);
            }

            while (this.commandList.Count > 0 && (this.UndoCount > this.commandsLimit - 1))
            {
                this.commandList.RemoveAt(0);
                --this.lastUndoIndex;
            }

            if (this.commandsLimit > this.commandList.Count)
            {
                this.commandList.Add(command);
                ++this.lastUndoIndex;
            }

            this.OnCommandsChanged();
        }

        /// <summary>
        /// Gets a ToolTip for Undo command.
        /// </summary>
        internal string UndoToolTip 
        {
            get 
            {
                string toolTip = string.Empty;

                for (int i = this.lastUndoIndex; i >= 0; --i)
                {
                    string command = this.commandList[i].ToString();
                    toolTip += command.Replace("ZZero.ZPlanner.Commands.", "") + "\n";
                }

                return toolTip;
            }
        }

        /// <summary>
        /// Gets a ToolTip for Undo command.
        /// </summary>
        internal string RedoToolTip 
        {
            get 
            {
                string toolTip = string.Empty;

                for (int i = this.lastUndoIndex + 1; i < this.commandList.Count; ++i)
                {
                    string command = this.commandList[i].ToString();
                    toolTip += command.Substring(command.LastIndexOf(".") + 1) + "\n";
                }

                return toolTip;
            }
        }
    }
}
