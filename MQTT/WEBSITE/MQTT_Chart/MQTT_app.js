$(document).ready(function(){
  $.ajax({
    // location of the datafile
    url: "https://zellawaltman.org/MQTT_Chart/MQTT_data.php",
    method: "GET",
    success: function(data) {
      console.log(data);
      
      var x_axis = []; // a generic variable
      var y_axis = [];

      for(var i in data) {
        x_axis.push("N:" + data[i].time_received); // must match your dBase columns
        y_axis.push(data[i].potentiometer_value); // Potentiometer value (V)
      }

      var chartdata = {
        labels: x_axis,
        datasets : [
          {
            label: 'Potentiometer Values (V)', //Title
            // Change colors: https://www.w3schools.com/css/tryit.asp?filename=trycss3_color_rgba
            //backgroundColor: 'rgba(200, 200, 200, 0.75)',
            borderColor: 'rgba(62, 21, 94, 1)',
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
                    text: "Potentiometer Values (V) vs. Time"
                }
            },
       
            scales: {
                x: {
                    title: {display: true, text: "Time of Measurement"},
                    ticks: {display: false}
                },
                y: {
                    title: {display: true, text: "Potentiometer Values (V)"}
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
