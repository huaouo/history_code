using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace NetFinal.Models
{
    [Table("user")]
    public partial class User
    {
        [Column("id")] public string Id { get; set; }
        [Required] [Column("password")] public string Password { get; set; }
    }
}