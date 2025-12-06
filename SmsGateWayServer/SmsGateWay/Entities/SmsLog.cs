using System.ComponentModel.DataAnnotations;

namespace SmsGateWay.Entities
{
    public class SmsLog
    {
        [Key]
        public int Id { get; set; }
        public string? Sender { get; set; }     
        public string? Message { get; set; }    
        public DateTime ReceivedAt { get; set; }  
        public int SignalQuality { get; set; } 
    }
}
