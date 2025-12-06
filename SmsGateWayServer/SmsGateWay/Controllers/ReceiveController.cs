using Microsoft.AspNetCore.Mvc;
using SmsGateWay.Context;
using SmsGateWay.Entities;

namespace SmsGateWay.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ReceiveController : Controller
    {
        private readonly AppDbContext _context;

        public ReceiveController(AppDbContext context)
        {
            _context = context;
        }
        [HttpGet]
        public IActionResult Get(int input)
        {
            var response = input * 2;
            return Ok(response);
        }

        [HttpPost]
        public IActionResult Post([FromBody] SmsLog sms)
        {
            _context.SmsLogs.Add(sms);
            _context.SaveChanges();

            return Ok(new { success = true ,sms.Id});
        }
    }
}
