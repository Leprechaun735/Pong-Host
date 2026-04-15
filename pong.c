#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "get your own ssid, you cant have mine";
const char* password = "password";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Pong</title>
  <style>
    body { background: #222; color: #fff; text-align: center; font-family: sans-serif; }
    canvas { background: #000; display: block; margin: auto; }
    h1 { margin-top: 20px; }
  </style>
</head>
<body>
  <h1>ESP32 Pong</h1>
  <canvas id="pong" width="600" height="400"></canvas>
  <script>
    const canvas = document.getElementById('pong');
    const ctx = canvas.getContext('2d');

    let paddleHeight = 80, paddleWidth = 10;
    let playerY = canvas.height/2 - paddleHeight/2;
    let aiY = canvas.height/2 - paddleHeight/2;
    let ballX = canvas.width/2, ballY = canvas.height/2;
    let ballRadius = 8;
    let ballSpeedX = 4, ballSpeedY = 4;

    let upPressed = false, downPressed = false;

    document.addEventListener('keydown', (e) => {
      if(e.key == 'ArrowUp') upPressed = true;
      if(e.key == 'ArrowDown') downPressed = true;
    });
    document.addEventListener('keyup', (e) => {
      if(e.key == 'ArrowUp') upPressed = false;
      if(e.key == 'ArrowDown') downPressed = false;
    });

    function drawPaddle(x, y){
      ctx.fillStyle = '#fff';
      ctx.fillRect(x, y, paddleWidth, paddleHeight);
    }

    function drawBall(){
      ctx.beginPath();
      ctx.arc(ballX, ballY, ballRadius, 0, Math.PI*2);
      ctx.fillStyle = '#fff';
      ctx.fill();
      ctx.closePath();
    }

    function draw(){
      ctx.clearRect(0, 0, canvas.width, canvas.height);

      // drawings
      drawPaddle(10, playerY);
      drawPaddle(canvas.width - 20, aiY);
      drawBall();

      // Ball
      ballX += ballSpeedX;
      ballY += ballSpeedY;

      // wall
      if(ballY + ballRadius > canvas.height || ballY - ballRadius < 0){
        ballSpeedY = -ballSpeedY;
      }

      // Collision 
      if(ballX - ballRadius < 20 && ballY > playerY && ballY < playerY + paddleHeight){
        ballSpeedX = -ballSpeedX;
      }
      if(ballX + ballRadius > canvas.width - 20 && ballY > aiY && ballY < aiY + paddleHeight){
        ballSpeedX = -ballSpeedX;
      }

      // robot control
      if(aiY + paddleHeight/2 < ballY) aiY += 3;
      if(aiY + paddleHeight/2 > ballY) aiY -= 3;

      // Player controls
      if(upPressed && playerY > 0) playerY -= 5;
      if(downPressed && playerY < canvas.height - paddleHeight) playerY += 5;

      requestAnimationFrame(draw);
    }

    draw();
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {

}