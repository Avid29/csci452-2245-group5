// Program for showing off custom Nunchuk PCB
// Shaun Thornton

import processing.serial.*;

class Player
{
  private final color c;
  private int x, y, radius;
  private boolean present;
  
  public Player(int x, int y, color c)
  {
    this.c = c;
    this.x = x;
    this.y = y;
    this.radius = 50;
    this.present = false;
  }
  
  public boolean isPresent()
  {
    return this.present;
  }
  
  public void setPresent(boolean present)
  {
    this.present = present;
  }
  
  public void update(int deltaX, int deltaY, int grow, int shrink)
  {
    if(this.present)
    {
      this.x += deltaX;
      this.y += deltaY;
      this.radius += grow;
      this.radius -= shrink;
    }
  }
  
  public void render()
  {
    fill(present ? c : 0);
    noStroke();
    circle(x, y, radius);
  }
}

Player[] players;

Serial serialPort;
byte[][] inputData = new byte[4][7];

void setup()
{
  fullScreen();
  
  players = new Player[] {
    new Player((int)random(width), (int)random(height), color(255, 0, 0)),
    new Player((int)random(width), (int)random(height), color(0, 255, 255)),
    new Player((int)random(width), (int)random(height), color(0, 255, 0)),
    new Player((int)random(width), (int)random(height), color(255, 0, 255)),
  };
  
  serialPort = new Serial(this, "COM5", 115200);
  delay(3000);
  serialPort.clear();
}

void draw()
{
  background(50, 50, 50);
  
  // Read input
  if(serialPort.available() >= 4*7)
  {
    for(int i = 0; i < players.length; i++)
    {
      // Six data bytes follow
      serialPort.readBytes(inputData[i]);
        
      // Check if the current nunchuk is plugged in
      players[i].setPresent(inputData[i][0] > 0);
        
      // Update the current player
      int deltaX = (inputData[i][1] & 0xFF) - 127;
      int deltaY = -((inputData[i][2] & 0xFF) - 127);
      
      if(abs(deltaX) < 10) deltaX = 0;
      if(abs(deltaY) < 10) deltaY = 0;
      
      deltaX /= 4;
      deltaY /= 4;
      
      players[i].update(deltaX, deltaY, ((~inputData[i][6]) & 1)*4, ((~inputData[i][6] >> 1) & 1)*4);
    }
  }
  
  // Update and render
  for(Player player : players)
  {
    player.render();
  }
}
