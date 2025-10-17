$(document).ready(function(){
  $.ajax({
    url: "https://zellawaltman.org/Chartjs/data.php", // location of the datafile
    method: "GET",
    success: function(data) {
      console.log(data);
      var x_axis = []; // a generic variable
      var y_axis = [];

      for(var i in data) {
        x_axis.push("N:" + data[i].time_received); // must match your dBase columns
        y_axis.push(data[i].temperature);
      }

      var chartdata = {
        labels: x_axis,
        datasets : [
          {
            label: 'Node 1 Temperature', //Title
            // Change colors: https://www.w3schools.com/css/tryit.asp?filename=trycss3_color_rgba
            //backgroundColor: 'rgba(200, 200, 200, 0.75)',
            borderColor: 'rgba(27, 133, 55, 1)',
            hoverBackgroundColor: 'rgba(200, 200, 200, 1)',
            hoverBorderColor: 'rgba(200, 200, 200, 0)',
            data: y_axis
          }
        ]
      };

      var ctx = document.getElementById('mycanvas').getContext('2d');

      var barGraph = new Chart(ctx, {
        type: 'line',   //Chart Type
        data: chartdata,
        options: {
            responsive: true,
           
            maintainAspectRatio: true,
           
            plugins: {
                title: {
                    display: true,
                    text: "Node 1"
                }
            },
       
            scales: {
                x: {
                    title: {display: true, text: "Time of Measurement"},
                    ticks: {display: false}
                },
                y: {
                    title: {display: true, text: "Temperature (°C)"}
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
