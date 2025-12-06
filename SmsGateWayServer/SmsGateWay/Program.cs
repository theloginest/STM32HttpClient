using Microsoft.EntityFrameworkCore;
using SmsGateWay.Context;

var builder = WebApplication.CreateBuilder(args);
//var builder = WebApplication.CreateBuilder(args); builder.WebHost.UseUrls("http://0.0.0.0:8000");

// Add services to the container.

builder.Services.AddControllers();
// Learn more about configuring OpenAPI at https://aka.ms/aspnet/openapi
builder.Services.AddDbContext<AppDbContext>(options =>
    options.UseSqlServer(builder.Configuration.GetConnectionString("DefaultConnection")));


var app = builder.Build();

// Configure the HTTP request pipeline.


app.UseHttpsRedirection();

app.UseAuthorization();

app.MapControllers();

app.Run();
