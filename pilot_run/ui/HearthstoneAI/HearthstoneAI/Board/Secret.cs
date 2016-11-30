﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Secret
    {
        [DataMember]
        public string card_id;

        [DataMember]
        public string @class;

        public bool Parse(GameState game, GameState.Entity entity)
        {
            bool ret = true;

            this.card_id = entity.CardId;
            if (entity.HasTag(GameTag.CLASS) == false)
            {
                this.@class = "";
            }
            else
            {
                this.@class = ((TAG_CLASS)entity.GetTag(GameTag.CLASS)).ToString();
            }

            return ret;
        }

        public override bool Equals(object obj)
        {
            Secret rhs = obj as Secret;
            if (rhs == null) return false;
            if (!this.card_id.Equals(rhs.card_id)) return false;
            if (!this.@class.Equals(rhs.@class)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.card_id,
                this.@class
            }.GetHashCode();
        }
    }
}