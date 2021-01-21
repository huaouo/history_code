using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using Newtonsoft.Json;

namespace NetFinal.Models
{
    [Table("class")]
    public partial class Class
    {
        public Class()
        {
            Student = new HashSet<Student>();
        }

        [Key] [Column("class")] public string ClassName { get; set; }
        [Required] [Column("college")] public string College { get; set; }

        [JsonIgnore]
        [InverseProperty("ClassNavigation")]
        public virtual ICollection<Student> Student { get; set; }
    }
}