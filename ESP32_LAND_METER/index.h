#ifndef INDEX_H_
#define INDEX_H_

const char home0_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang="it">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=yes">
      <title>Land Meter by Dani</title>
      <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
      
      <style>
      </style>
    </head>
  <body>
  <h1 style="text-align:center;">Land Meter</h1>
  <h2 style="text-align:center;">By Dani</h2>

  <br />
  <p>LAT:<span id="lat"></span</p>
  <p>LON:<span id="lon"></span</p>
  <p>ALT:<span id="alt"></span</p>

    <div>
        <iframe width="600" height="500" id="maps" class="maps" src="https://www.google.com/maps/d/u/0/embed?mid=1OWjIiXc7clGxGdbrHvjNOdY9ZLo&ll=44.62900381580296%2C10.948825550797263&z=18" frameborder="0" scrolling="no" marginheight="0" marginwidth="0">
        </iframe>
        <br>
    </div>

    <script>    
      setInterval(function() {
        getLink();
        getLon();
        getLat();
        getAlt();
      }, 3500); 
      
      function getLink() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("maps").src = this.responseText;
          }
        };
        xhttp.open("GET", "getPos", true);
        xhttp.send();
      }

      function getLon() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("lon").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "getLat", true);
        xhttp.send();
      }

      function getLat() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("lat").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "getLon", true);
        xhttp.send();
      }

      function getAlt() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("alt").innerHTML = this.responseText;
          }
        };
        xhttp.open("GET", "getAlt", true);
        xhttp.send();
      }
    </script>

    <script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
  </body>
</html>
)rawliteral";

#endif // INDEX_H_
