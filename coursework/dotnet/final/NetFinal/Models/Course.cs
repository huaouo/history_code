using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using Newtonsoft.Json;

namespace NetFinal.Models
{
    [Table("course")]
    public partial class Course
    {
        [Column("id")] public string Id { get; set; }
        [Column("course")] public string CourseName { get; set; }
        [Column("score")] public long Score { get; set; }

        [JsonIgnore]
        [ForeignKey("Id")]
        [InverseProperty("Course")]
        public virtual Student IdNavigation { get; set; }
    }
}