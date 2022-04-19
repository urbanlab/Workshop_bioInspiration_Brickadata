import processing.serial.*;

float def_width = 1920;
float def_height = 1080;
int wwidth = 1280;
int wheight = 720;

float ratio;

int fps = 30;

//

float left_width;
float right_width;

//

Serial _serial;
int lf = 10;

//

int grid_size = 86;
int grid_padding = 8; 

int grid_width = 4; 
int grid_height = 4; 

int datasize = grid_width*grid_height*2;
int[] DATA = new int[datasize];

//

int n_data = 6;
String[] data_str = new String[n_data];

float[] data_trans = new float[n_data];
float[] data_speed = new float[n_data];


//

PFont font_number;
PFont font_bold;
PFont font_light;

//

float cube_rotation = 0;

//

boolean data_changed = false;
int data_changed_type = 0;
int frame_changed = 0;

boolean show_splash = false;
int frame_splash = 0;

// 

color yellow = color(255,224,0);
color blue = color(0, 196, 255);

//color black = color(32,32,32);
//color grey = color(64,64,64);
//color grey2 = color(128,128,128);

color black = color(255,235,200);
color grey = color(255,195,100);
color grey2 = color(255,180,50);

//

int transition = (int)fps / 2;
boolean draw_main = true;

void setup()
{
  // window

  boolean fullscreen = true;

  fullScreen(P3D);
  //size(1280,720,P3D);

  if (fullscreen)
  {
    ratio =1;
  }
  else
  {
    ratio = min( wwidth/def_width, wheight/def_height );
  }
  
  frameRate(fps);
  smooth(8);

  //

  left_width = width / 3;
  right_width = (width / 3) * 2;

  // grid

  grid_size *= ratio;
  grid_padding *= ratio;
  
  // serial

  printArray(Serial.list());
  String port_name = Serial.list()[1];
  _serial = new Serial(this, port_name, 115200);
  _serial.clear();
  
  // init data

  for (int i = 0; i < datasize; i++)
  {
    DATA[i] = 0;
  }

  data_str[0] = "PARTICIPANTS ";
  data_str[1] = "COCCINELLES PONCTUELLES ";
  data_str[2] = "CHRYSALIDES CONCENTRÉES ";
  data_str[3] = "ARAIGNÉES SOCIABLES ";
  data_str[4] = "PAPILLONS CURIEUX ";
  data_str[5] = "D'ENGAGEMENT ";

  data_speed[0] = 1.5 *2;
  data_speed[1] = 2.5 *2;
  data_speed[2] = 2 *2;
  data_speed[3] = 3 *2;
  data_speed[4] = 1.5 *2;
  data_speed[5] = 2.5 *2;

  for (int i = 0; i < n_data; i++)
  {
    data_trans[i] = 0;
  }

  // drawing

  fill(255);
  noStroke();

  // fonts

  // font_number = loadFont("data/fonts/Miso-Bold-48.vlw");
  font_number = loadFont("data/fonts/TheMixBold-Plain-96.vlw");
  font_bold = loadFont("data/fonts/TheMixBold-Plain-96.vlw");
  font_light = loadFont("data/fonts/TheMixLight-Plain-48.vlw");

}

void drawLeft()
{

  hint(DISABLE_DEPTH_TEST);

  fill(black);
  rect(0,0,left_width,height);

  fill(grey2);

  float padding_lr = 32 * ratio;
  float header_height = 64 * ratio;

  float font_size = 32 * ratio;
  textFont(font_bold, font_size);
  float font_height = ((textAscent() + textDescent())/2) *1.4;


  String text = "Brickadata";
  text(text, padding_lr, header_height/2 + font_height/2 ); 

  hint(ENABLE_DEPTH_TEST);

  pushMatrix();

  fill(255);
  noStroke();

  lights();
  
  float far = 10000;
  ortho(-width/2.0, width/2.0, -height/2.0, height/2.0, 2, far);
  translate(left_width/2, height/2, 0);
  rotateX(-PI/6);
  rotateY(PI/3*(cube_rotation/200.f));
  cube_rotation++;

  int offset_x = -(grid_width*grid_size + (grid_width-1)*grid_padding ) /2 + (grid_size/2);
  int offset_y = -(grid_height*grid_size + (grid_height-1)*grid_padding ) /2 + (grid_size/2);
  int offset_z = (10*grid_size + (10-1)*grid_padding ) /4;

  for ( int j = 0; j < grid_height; j++ )
  {
    for ( int i = 0; i < grid_width; i++ )
    {
      int pos = j*grid_width + i;
      int n1 = DATA[pos*2];
      int n2 = DATA[pos*2+1];
      
      pushMatrix();
      int x = offset_x+i*(grid_size+grid_padding);
      int y = offset_y+j*(grid_size+grid_padding);
      translate(x,0,y);

      fill(black);

      pushMatrix();
      int z = grid_padding + offset_z + (grid_size/2);
      translate(0,z,0);
      box(grid_size, grid_padding, grid_size);
      popMatrix();

      int ck = 0;
      for ( int k = 0; k < n1; k++ )
      {
        fill(blue);

        pushMatrix();
        z = -ck*(grid_size+grid_padding) + offset_z;
        translate(0,z,0);
        box(grid_size);
        popMatrix();
        ck++;
      }
      
      for ( int k = 0; k < n2; k++ )
      {
        fill(yellow);

        pushMatrix();
        z = -ck*(grid_size+grid_padding) + offset_z;
        translate(0,z,0);
        box(grid_size);
        popMatrix();
        ck++;
      }

      popMatrix();
    }
  }

  noLights();

  popMatrix();
}

void drawRight()
{  

  hint(DISABLE_DEPTH_TEST);

  pushMatrix();

  translate(left_width, 0, 0);

  fill(grey);
  line(0, 0, 0, height);

  fill(grey2);

  float padding_lr = 32 * ratio;
  float header_height = 64 * ratio;

  float font_size = 32 * ratio;
  textFont(font_bold, font_size);
  float font_height = ((textAscent() + textDescent())/2) *1.4;


  String text = "Ce mois-ci à Tuba";
  text(text, padding_lr, header_height/2 + font_height/2 ); 

  translate(0, header_height, 0);

  //

  float lines_height = height - header_height;
  float line_height = lines_height / n_data;


  
  for ( int i = 0; i < n_data; i++ )
  {
    color bg = black;
    color fg = grey2;
    color fg_2 = color(64,64,64);
  
    if ( data_changed && frame_changed < n_data*transition )
    {
      frame_changed++;

      if ( frame_changed/transition >= i && frame_changed/transition < i+1 )
      {
         if (data_changed_type == 0 ) { bg = blue; }
         else { bg = yellow; }

         fg = black;
         fg_2 = black;
      }
    }

    int value = 0;
    if ( i == 0 ) { value = getData01(); }
    else if ( i == 1 ) { value = getData02(); }
    else if ( i == 2 ) { value = getData03(); }
    else if ( i == 3 ) { value = getData04(); }
    else if ( i == 4 ) { value = getData05(); }
    else if ( i == 5 ) { value = getData06(); }

    float y = line_height*i;

    fill(bg);
    noStroke();
    rect(0,y,right_width, line_height);

    fill(fg);

    y += line_height/2;

    float x = -data_trans[i] + padding_lr;
    float text_width = 0;

    for (int j = 0; j < 3; j++ )
    {
      font_size = 96 * ratio;
      textFont(font_number, font_size);
      font_height = ((textAscent() + textDescent())/2) *1.4;

      String text_value = value+" ";
      text_value = text_value.replace('0', 'O');
      float text_value_width = textWidth("OO ");
      if ( i == 0 ) 
      { 
        text_value_width = textWidth("OO ");
      }
      if ( i == 5 ) 
      { 
        text_value = value + "% "; 
        text_value = text_value.replace('0', 'O');
        text_value_width = textWidth("OOO% ");
      }

      float offset_x_value = text_value_width - textWidth(text_value);

      fill(fg_2);

      text( text_value, x+offset_x_value, y + font_height/2 );
      x += text_value_width;
      text_width = text_value_width;

      font_size = 96 * ratio;
      textFont(font_bold, font_size);
      font_height = ((textAscent() + textDescent())/2) *1.4;

      fill(fg);

      text = data_str[i];
      text( text, x, y + font_height/2 );
      x += textWidth(text);
      text_width += textWidth(text);
    }

    data_trans[i] += data_speed[i] / 4.0; 
    if ( data_trans[i] >= text_width )
    {
      data_trans[i] = 0;
    }
  }

  for ( int i = 0; i < n_data; i++ )
  {
    stroke(grey);
    float y = line_height*i;
    line(0, y, right_width, y);
  }

  popMatrix();
}

void draw()
{  
  if (_serial.available() >0)
  {
    String val = _serial.readStringUntil(lf);
    if ( val != null ) 
    {
      JSONArray json = parseJSONArray(val);
      if (json != null && json.size() >= datasize) 
      {  
        for (int i = 0; i < datasize; i++)
        {
          if ( DATA[i] < json.getInt(i) )
          {
            data_changed = true;
            data_changed_type = i%2;
            if (!show_splash)
            {
              frame_changed = 0;
            }
          }
          DATA[i] = json.getInt(i);
        }
      }
    }
  }
  
  background(black);

  if (draw_main)
  {
    drawRight();
    drawLeft(); 
  }

  if ( frame_changed == n_data*transition )
  {
    frame_splash = 0;
    frame_changed++;
    show_splash = true;
  }

  if ( show_splash )
  {
    drawSplash();
  }
  else
  {
      draw_main = true;
  }
}

void drawSplash()
{
  hint(DISABLE_DEPTH_TEST);

  float alpha = 1;
  if ( frame_splash <= transition )
  {
    alpha = map(frame_splash,0,transition,0,1);
  }
  else if ( frame_splash >= transition*5 )
  {
    alpha = map(frame_splash,transition*5,transition*6,1,0);
  }

  fill(black,alpha*255);
  rect(0,0,width,height);


  draw_main = true;

  if ( frame_splash >= transition &&  frame_splash <= transition*5 )
  {

    draw_main = false;

    pushMatrix();

    hint(ENABLE_DEPTH_TEST);

    color col = yellow;
    if (data_changed_type == 0 ) { col = blue; }

    fill(col,alpha*255);

    lights();

    float far = 10000;
    ortho(-width/2.0, width/2.0, -height/2.0, height/2.0, 2, far);
    translate(width/2, height/2, 0);
    rotateX(-PI/6);
    rotateY(PI/3*(cube_rotation/200.f));
    cube_rotation++;

    box(grid_size*4);    

    popMatrix();

    fill(grey2);

    float font_size = 128 * ratio;
    textFont(font_bold, font_size);
    float font_height = ((textAscent() + textDescent())/2) *1.4;


    String text = "Bienvenue à Tuba !";
    float text_width = textWidth(text);
    text(text, width/2 - text_width/2, height/2 + grid_size*5 ); 
  }

  frame_splash++;
  if ( frame_splash > transition*6 )
  {
    show_splash = false;
    frame_splash = 0;
    frame_changed = 0;
    data_changed = false;
  }
}

int getData01()
{
  int sum = 0;
  for ( int i = 0; i < datasize; i++ )
  {
    sum += DATA[i];
  }
  return sum;
}

int getData02()
{
  int sum = 0;
  for ( int i = 0; i < grid_width*2; i++ )
  {
    sum += DATA[i];
  }
  return sum;
}

int getData03()
{
  int sum = 0;
  for ( int i = 0; i < grid_width*2; i++ )
  {
    sum += DATA[i+grid_width*2];
  }
  return sum;
}

int getData04()
{
  int sum = 0;
  for ( int i = 0; i < grid_width*2; i++ )
  {
    sum += DATA[i+grid_width*2*2];
  }
  return sum;
}

int getData05()
{
  int sum = 0;
  for ( int i = 0; i < grid_width*2; i++ )
  {
    sum += DATA[i+grid_width*2*3];
  }
  return sum;
}

int getData06()
{
  int sum = 0;
  float sum_intensity = 0;

  for ( int i = 0; i < datasize; i++ )
  {
    int pos = i/2;
    int intensity = pos%4;
    sum += DATA[pos*2];
    sum += DATA[pos*2+1];

    // float data_intensity = (DATA[pos*2]+DATA[pos*2+1]) * (intensity/4.0);
    sum_intensity += intensity*0.25 + 0.25;
  }

  sum_intensity *= 100;
  sum_intensity /= sum;

  return sum;
}

void keyPressed() 
{
  if ( keyCode == UP )
  {
    DATA[0]++;
    data_changed = true;
    data_changed_type = (int)random(0,2);
    if (!show_splash)
    {
      frame_changed = 0;
    }
  }
  else if ( keyCode == DOWN )
  {
    DATA[0]--;
    data_changed = true;
    data_changed_type = (int)random(0,2);
    if (!show_splash)
    {
      frame_changed = 0;
    }
  }
}
