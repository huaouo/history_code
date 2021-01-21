using System;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using NetFinal.Models;

namespace NetFinal.Services
{
    public class UserService
    {
        private readonly FinalContext _dbContext;

        public UserService(FinalContext dbContext)
        {
            _dbContext = dbContext;
        }

        public bool ValidateByPassword(string username, string password)
        {
            return ValidateByPasswordHash(username, GetPasswordHash(password));
        }

        public string GetPasswordHash(string password)
        {
            using (SHA256 sha = new SHA256Managed())
            {
                var hashBytes = sha.ComputeHash(Encoding.UTF8.GetBytes(password));
                return BitConverter.ToString(hashBytes).Replace("-", "");
            }
        }

        public bool ValidateByPasswordHash(string username, string passwordHash)
        {
            if (string.IsNullOrEmpty(username) || string.IsNullOrEmpty(passwordHash))
                return false;
            var hashFromDb = (from rec in _dbContext.User
                where rec.Id == username
                select rec.Password).FirstOrDefault();
            return hashFromDb == passwordHash;
        }
    }
}