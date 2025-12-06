using Microsoft.EntityFrameworkCore;
using SmsGateWay.Entities;

namespace SmsGateWay.Context
{
    public class AppDbContext: DbContext
    {
        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            
        }
        public DbSet <SmsLog> SmsLogs { get; set; }
    }
}
