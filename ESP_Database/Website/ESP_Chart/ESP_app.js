$(document).ready(function(){
    
// read ?node=node_1 from URL
  const params = new URLSearchParams(location.search);
  const node = params.get("node") || "node_1";
  
  $.ajax({
    // location of the datafile
    url: "https://zellawaltman.org/ESP_Chart/ESP_data.php?node=" + encodeURIComponent(node) + "&t=" + Date.now(), // cache-buster
    method: "GET",
    success: function(data) {
      console.log(data);
      
      var x_axis = []; // a generic variable
      var y_temp = [];
      var y_hum = [];

      for(var i in data) {
        x_axis.push("N:" + data[i].time_received); // must match your dBase columns
        y_temp.push(data[i].temperature); // Temperature Line
        y_hum.push(data[i].humidity); // Humidity Line
      }

      var chartdata = {
        labels: x_axis,
        datasets : [
          {
            label: `Temperature - ${node}`, //Title
            // Change colors: https://www.w3schools.com/css/tryit.asp?filename=trycss3_color_rgba
            //backgroundColor: 'rgba(200, 200, 200, 0.75)',
            borderColor: 'rgba(27, 133, 55, 1)',
            hoverBackgroundColor: 'rgba(200, 200, 200, 1)',
            hoverBorderColor: 'rgba(200, 200, 200, 0)',
            data: y_temp
          },
          {
            label: `Humidity — ${node}`,
            borderColor: "rgba(55, 133, 200, 1)",
            borderDash: [6, 4],      // <-- dashed line
            data: y_hum,
            yAxisID: 'y2'            // second axis
          }
        ]
      };

      var ctx = document.getElementById('mycanvas').getContext('2d');

      var barGraph = new Chart(ctx, {
        type: 'line',   //Chart Type
        data: chartdata,
        options: {
            responsive: true,
            
            // Turn Off Animations for better looking constant reloads
            animation: false,             
            transitions: {
                active: { animation: { duration: 0 } },
                resize: { animation: { duration: 0 } }
            },
           
            maintainAspectRatio: false,
           
            plugins: {
                title: {
                    display: true,
                    text: node
                }
            },
       
            scales: {
                x: {
                    title: {display: true, text: "Time of Measurement"},
                    ticks: {display: false}
                },
                y: {
                    title: {display: true, text: "Temperature (°C)"}
                },
                y2: { 
                    title: { display: true, text: "Humidity (%)" },
                    position: 'right',
                    grid: { drawOnChartArea: false } 
                }
            }
        },
      });
    },
    error: function(data) {
      console.log(data);
    }
  });
});
