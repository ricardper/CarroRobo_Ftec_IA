// web.h
#ifndef _WEB_TRAB_H

#define _WEB_TRAB_H

static const char *htmlHomePage = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>
    .arrows {
      font-size:30px;
      color:red;
    }
    td.button {
      background-color:black;
      border-radius:25%;
      box-shadow: 5px 5px #888888;
    }
    td.button:active {
      transform: translate(5px,5px);
      box-shadow: none; 
    }

    .noselect {
      -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
         -khtml-user-select: none; /* Konqueror HTML */
           -moz-user-select: none; /* Firefox */
            -ms-user-select: none; /* Internet Explorer/Edge */
                user-select: none; /* Non-prefixed version, currently
                                      supported by Chrome and Opera */
    }

    .slidecontainer {
      width: 100%;
    }

    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 15px;
      border-radius: 5px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }

    .slider:hover {
      opacity: 1;
    }
  
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    .slider::-moz-range-thumb {
      width: 25px;
      height: 25px;
      border-radius: 50%;
      background: red;
      cursor: pointer;
    }

    </style>
  
  </head>
  <body class="noselect" align="center" style="background-color:white">
    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr>
        <img id="cameraImage" src="" style="width:400px;height:250px"></td>
      </tr> 

      <tr>
        <td></td>
        <td class="button"
        ontouchstart='sendButtonInput("frente","1")'
         ontouchend='sendButtonInput("frente","-1")' 
        onmousedown='sendButtonInput("frente","1")' 
        onmouseup='sendButtonInput("frente","-1")'        
        ><span class="arrows" >&#8679;</span></td>
        <td></td>
      </tr>

      <tr>
        <td class="button" 
        ontouchstart='sendButtonInput("esquerda","3")'
         ontouchend='sendButtonInput("esquerda","-3")' 
         onmousedown='sendButtonInput("esquerda","3")' 
        onmouseup='sendButtonInput("esquerda","-3")'
        > <span class="arrows" >&#8678;</span></td>

        <td class="button"
        ontouchstart='sendButtonInput("parar","0")'
         ontouchend='sendButtonInput("parar","0")' 
         onmousedown='sendButtonInput("parar","0")' 
        onmouseup='sendButtonInput("parar","0")'
        ><span class="arrows" >&#9634;</span></td> 

        <td class="button" 
        ontouchstart='sendButtonInput("direita","4")' 
        ontouchend='sendButtonInput("direita","-4")' 
        onmousedown='sendButtonInput("direita","4")'  
        onmouseup='sendButtonInput("direita","-4")' 
        ><span class="arrows" >&#8680;</span></td></tr>

      <tr>
        <td></td>
        <td class="button" 
        ontouchstart='sendButtonInput("traz","2")' 
        ontouchend='sendButtonInput("traz","-2")'
        onmousedown='sendButtonInput("traz","2")' 
        onmouseup='sendButtonInput("traz","-2")'        
        > <span class="arrows" >&#8681;</span></td>
        <td></td>
      </tr>

      <tr/><tr/>
      <tr>
        <td style="text-align:left"><b>Velocidade:</b><span id="VelocidadeValor">0</span></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="-100" max="100" value="0" class="slider" id="Velocidade" oninput='sendButtonInput("Velocidade",value); updateSliderValue("Velocidade", value);'>
          </div>
        </td>
      </tr>        
      
      <tr>
        <td style="text-align:left"><b>Direcao:</b><span id="DirecaoValor">0</span></td>
        <td colspan=2>
         <div class="slidecontainer">
            <input type="range" min="30" max="140" value="85" class="slider" id="Direcao" oninput='sendButtonInput("Direcao",value) ; updateSliderValue("Direcao", value);  '>
          </div>
        </td>
      </tr> 
        
    </table>
  
    <script>
      var webSocketCameraUrl = "ws:\/\/" + window.location.hostname + "/Camera";
      var webSocketCarInputUrl = "ws:\/\/" + window.location.hostname + "/Carro";      
      var websocketCamera;
      var websocketCarInput;

      function updateSliderValue(key, value) {
          var valueSpan = document.getElementById(key + "Valor");
          if (valueSpan) {
              valueSpan.textContent = value;
          }
      }
      
      function initCameraWebSocket() 
      {
        websocketCamera = new WebSocket(webSocketCameraUrl);
        websocketCamera.binaryType = 'blob';
        websocketCamera.onopen    = function(event){};
        websocketCamera.onclose   = function(event){setTimeout(initCameraWebSocket, 2000);};
        websocketCamera.onmessage = function(event)
        {
          var imageId = document.getElementById("cameraImage");
          imageId.src = URL.createObjectURL(event.data);
        };
      }
      
      function initCarInputWebSocket() 
      {
        websocketCarInput = new WebSocket(webSocketCarInputUrl);
        websocketCarInput.onopen    = function(event)
        {
            sendButtonInput("Velocidade", document.getElementById("Velocidade").value);
          
            sendButtonInput("Direcao", document.getElementById("Direcao").value);
                            
          };
          websocketCarInput.onclose   = function(event){setTimeout(initCarInputWebSocket, 2000);};
          
          websocketCarInput.onmessage = function(event){

          var msg = event.data;
        var parts = msg.split(','); // Divide a mensagem em Chave e Valor (ex: ["Direcao", "90"])
        
        if (parts.length === 2) {
            var key = parts[0].trim();   // Chave: "Direcao" ou "Velocidade"
            var value = parts[1].trim(); // Valor: "90" ou "50"
            
            // Tenta encontrar o elemento pelo ID (que é o mesmo nome da Chave)
            var slider = document.getElementById(key);

            if (slider) {
                // Atualiza o valor do slider e o exibe (se fosse um elemento de texto)
                slider.value = value;
                updateSliderValue(key, value);
                
                // Opcional: Adicionar feedback visual ou log
                console.log("Slider atualizado: " + key + " para " + value);
            }
        }
    
        
        
        };        
      }
      
      function initWebSocket() 
      {
        initCameraWebSocket ();
        initCarInputWebSocket();
      }

      function sendButtonInput(key, value) 
      {
        var data = key + "," + value;
        websocketCarInput.send(data);
      }
    
      window.onload = initWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });      
    </script>
  </body>    
</html>


)HTMLHOMEPAGE";

#endif