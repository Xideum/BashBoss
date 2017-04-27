#include <Tinyfont.h>
#include <ArduboyTones.h>
#include <Arduboy2.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

int gamestate = 0;
int playstate = 0;
int laststate = 0;

int counter = 0;

int upbuf = 0;
int downbuf = 0;
int abuf = 0;
int bbuf = 0;

int batselect = 0;
int pitchselect = 0;
int homebattingindex = 0;
int awaybattingindex = 0;
int batterindex = 0;
bool homebatting = false;

int bashinning = 1;
bool bashhomebatting = false;
int bashball = 0;
int bashstrike = 0;
int bashout = 0;
int bashlikelihood[4];

/*
 * bashlikelihood indexes:
 * 0 == hit
 * 1 == strike
 * 2 == ball
 * 3 == index (0 - 2)
 */
int bashresult = 0;
/*
 * bashresult:
 * 0 == null
 * 1 == hit
 * 2 == strike
 * 3 == ball
 * 4 == homerun
 * 5 == bean/walk
 * 
 */

bool infoshowhome = true;
int infoindex = 0;

bool playerhome = false;
char batting;

int homepitcher = 0;
int awaypitcher = 0;

int animation = 0;
bool twoplayers = false;

class Field
{
  public:
    int first = 0;
    int second = 0;
    int third = 0;  
} field;

class Basher
{
  public :
    int stamina;
    
    // batting
    int contact;
    int power;
    int eye;
  
    // fielding
    int def;
  
    // pitching
    int velocity;
    int control;

    // basher #
    int number;

    int handed;
    int pos;
    /*
     * positions:
     * 0 = c
     * 1 = 1b
     * 2 = 2b
     * 3 = 3b
     * 4 = ss
     * 5 = lf
     * 6 = cf
     * 7 = rf
     * 8 = sp
     * 9 = rp
     * 
     * default batting order
     * 1 - lf
     * 2 - cf
     * 3 - 3b
     * 4 - rf
     * 5 - 1b
     * 6 - 2b
     * 7 - c
     * 8 - p
     * 9 - ss
     */

    set (int, int, int, int, int, int, int, int, int, int);
};

Basher::set(int instamina, int incontact, int inpower, int ineye,
  int indef, int invelocity, int incontrol, int innumber, int inhanded, int inpos)
{
  stamina = instamina;
  contact = incontact;
  power = inpower;
  eye = ineye;
  def = indef;
  velocity = invelocity;
  control = incontrol;  
  number = innumber;
  handed = inhanded;
  pos = inpos;
}

//Basher* createteam(int skill)
void createteam(Basher *team)
{
  //Basher team[25];

   for (int i = 0; i < 11; i++)
  {
    int hand;
    if (random(0,100) < 75)
      hand = 0;
    else if (random(0,11) > 9)
      hand = 2;
    else
      hand = 1;


    switch (i)
    {

      /*
       * Basher::set(int instamina, int incontact, int inpower, int ineye,
  int indef, int invelocity, int incontrol, int innumber, int inhanded, int inpos)
       */

      // SP
      case 0:
        team[i].set(random(5,10), random(1,5), random(1,5), random(1,5),
          random(5,10), random(5,8), random(5,10), random(1,10), hand, 8);
        break;

      // RP
      case 1:
        team[i].set(random(1,5), random(1,5), random(1,5), random(1,5),
          random(5,10), random(7,10), random(5,10), random(11,16), hand, 9);
        break;

      case 2:
        team[i].set(random(0,5), random(5,10), random(5,10), random(5,10),
          random(5,10), random(7,10), random(5,10), random(16,20), hand, 9);
        break;

      // Ca
      case 3:
        team[i].set(random(5,10), random(5,10), random(5,10), random(5,10),
          random(5,10), random(3,8), random(3,8), random(21,30), hand, 0);
        break;

      // 1B
      case 4:
        team[i].set(random(5,10), random(5,10), random(5,10), random(5,10),
          random(5,10), random(3,8), random(3,8), random(31,40), hand, 1);
        break;

      // 2B
      case 5:
        team[i].set(random(5,10), random(5,10), random(5,10), random(5,10),
          random(5,10), random(3,8), random(3,8), random(41,50), hand, 2);
        break;

      // 3B
      case 6:
        team[i].set(random(5,10), random(5,10), random(5,10), random(5,10),
          random(5,10), random(3,8), random(3,8), random(51,60), hand, 3);
        break;

      // SS
      case 7:
        team[i].set(random(5,10), random(5,10), random(5,10), random(5,10),
          random(5,10), random(3,8), random(3,8), random(61,70), hand, 4);
        break;

      // LF
      case 8:
        team[i].set(random(5,10), random(7,10), random(7,10), random(5,10),
          random(5,10), random(5,10), random(3,8), random(71,80), hand, 5);
        break;

      // CF
      case 9:
        team[i].set(random(5,10), random(7,10), random(7,10), random(7,10),
          random(5,10), random(5,10), random(3,8), random(81,90), hand, 6);
        break;

      // RF
      case 10:
        team[i].set(random(5,10), random(7,10), random(7,10), random(7,10),
          random(5,10), random(5,10), random(3,8), random(91,100), hand, 7);
        break;
    }  
  }

  return team;
}

void drawField(int x, int y)
{
  // first
  arduboy.fillRect(x+32, y+6, 2, 2);
  if(field.first != 0)
  {
    arduboy.drawLine(x+32, y+6, x+32, y+3);
    arduboy.drawPixel(x+31, y+3);
  }

  // second
  arduboy.fillRect(x+17, y+2, 1, 1);
  if(field.second != 0)
  {
    arduboy.drawLine(x+17, y+2, x+17, y);
    arduboy.drawPixel(x+16, y);
  }
  
  // third
  arduboy.fillRect(x, y+6, 2, 2);
  if(field.third != 0)
  {
    arduboy.drawLine(x+1, y+6, x+1, y+3);
    arduboy.drawPixel(x, y+3);
  }

  // home
  arduboy.fillRect(x+15, y+16, 4, 4);

  // draw lines
  arduboy.drawLine(x, y+6, x+17, y+18);
  arduboy.drawLine(x+18, y+17, x+32, y+6);
}

void drawBasher(int x, int y, int frame = 0, bool right = true, char team = 'a')
{
  // body
  arduboy.fillRoundRect(x,y+4,14,12,1);

  
  // head
  arduboy.fillRoundRect(x+3,y,8,8,2);

  if (team == 'a')
  {
    for (int i = 0; i < 3; i++)
    {
      arduboy.drawLine(x+3+(3*i),y+4, x+3+(3*i),y+11, BLACK);
    }
  }
  
  if (right)
  {
    // hat brim
    arduboy.drawLine(x+11, y+2,x+15,y+2);
    
  }
  else 
  {
    // hat brim
    arduboy.drawLine(x-1, y+2,x+3,y+2);
  }

  switch (frame)
  {
    case 0:
      // feet
      arduboy.drawLine(x-2, y+16,x+2,y+16);
      arduboy.drawLine(x+11, y+16,x+15,y+16);
      break;

    case 1:

      break;
  }
  
}

void drawPitcher(int x, int y, int frame = 0, int handed = 0, bool team = true)
{
  // body
  arduboy.fillRoundRect(x,y+3,7,8,1);

    // head
  arduboy.fillRoundRect(x+2,y,4,4,2);

  if (!team)
  {
    for (int i = 0; i < 2; i++)
    {
      arduboy.drawLine(x+1+(2*i),y+2, x+1+(2*i),y+3, BLACK);
    }
  }
  
  if (handed == 0)
  {
    // hat brim
    arduboy.drawLine(x-1, y+1,x+2,y+1);
    
    
  }
  else 
  {
    // hat brim
    arduboy.drawLine(x+5, y+1,x+7,y+1);
  }

    switch (frame)
  {
    case 0:
      // feet
      arduboy.drawLine(x-1, y+11,x,y+11);
      arduboy.drawLine(x+6, y+11,x+7,y+11);
      break;

    case 1:

      break;
  }
}

void batbox()
{
  arduboy.fillRect(76, 2, 52, 32, WHITE);
  arduboy.fillRect(78, 4, 48, 28, BLACK);
  tinyfont.setCursor(88, 6);
  tinyfont.print("CONTACT");
  tinyfont.setCursor(88, 12);
  tinyfont.print("POWER");
  tinyfont.setCursor(88, 18);
  tinyfont.print("EYE");
  tinyfont.setCursor(88, 24);
  tinyfont.print("PINCH");
}

void changestate(int &state, int index)
{
  laststate = state;
  state = index;
  counter = 0;
}

void setup() {
  arduboy.begin();
  
  //srand(7/8);
  randomSeed(analogRead(A0));
  
  //Set the game to 60 frames per second
  arduboy.setFrameRate(60);
  
  arduboy.clear();
}
void loop() {
  if(!arduboy.nextFrame()) {
    return;
  }
  
  arduboy.clear();

  switch(gamestate) {
    case 0:
      // title
      arduboy.setCursor(64,8);
      arduboy.print("BASH");
      arduboy.setCursor(86,10);
      arduboy.print("BOSS");
      

     arduboy.fillCircle(24, 24, 16);
     arduboy.drawCircle(16, 16, 12, BLACK);
     arduboy.drawCircle(39, 41, 10, BLACK);

     if (infoindex == 0)
     {
      tinyfont.setCursor(64, 36);
      tinyfont.print("*");
     }
     else
     {
      tinyfont.setCursor(66, 42);
      tinyfont.print("*");
     }

     tinyfont.setCursor(68, 36);
     tinyfont.print("PLAY");

     tinyfont.setCursor(70, 42);
     tinyfont.print("ABOUT");

     if (arduboy.pressed(UP_BUTTON) == true && upbuf == 0)
      {
        upbuf = 1;
        if (infoindex == 1)
        {
          infoindex = 0;
        }
        else
        {
          infoindex = 1;
        }
      }
      else if (arduboy.pressed(DOWN_BUTTON) == true && downbuf == 0) 
      {
        downbuf = 1;
        if (infoindex == 0)
        {
          infoindex = 1;
        }
        else
        {
          infoindex = 0;
        }
      }

      if (arduboy.pressed(A_BUTTON) && abuf == 0) {
        abuf = 1;
        if (infoindex == 0)
          changestate(gamestate, 1);
          changestate(playstate, 0);
      }
      
      break;

    case 1:
      // gameplay
      Basher hometeam[11];
      Basher awayteam[11];

        switch (playstate)
        {
          case 0:
            // startup
            //hometeam = createteam(25);
            //awayteam = createteam(34);
            createteam(hometeam);
            createteam(awayteam);

            tinyfont.setCursor(8,8);
            tinyfont.print("CHOOSE YOUR SIDE");

            arduboy.setCursor(32, 48);
            arduboy.print("AWAY");

            arduboy.setCursor(84, 48);
            arduboy.print("HOME");

            if (infoindex == 0)
            {
              arduboy.drawLine(32,58, 56,58);
            }
            else
            {
              arduboy.drawLine(84,58, 108,58);
            }

            if (arduboy.pressed(LEFT_BUTTON) == true && upbuf == 0)
            {
              upbuf = 1;
              if (infoindex != 0)
              {
                infoindex = 0;
              }
            }
            else if (arduboy.pressed(RIGHT_BUTTON) == true && downbuf == 0) 
            {
              downbuf = 1;
              if (infoindex == 0)
              {
                infoindex = 1;
              }
            }

            if (arduboy.pressed(A_BUTTON) == true && abuf == 0)
            {
              abuf = 1;
              if (infoindex == 0)
                playerhome = false;
              else
                playerhome = true;

               changestate(playstate, 1);
            }

           
            
            break;

          case 1:
         
            // team setup

            batting = 'a';

            bashlikelihood[0] = 0;
            bashlikelihood[1] = 0;
            bashlikelihood[2] = 0;
            bashlikelihood[3] = 0;
            changestate(playstate, 2);
            break;

          case 2:
            // play
             /*      
           * default batting order
           * 1 - lf
           * 2 - cf
           * 3 - 3b
           * 4 - rf
           * 5 - 1b
           * 6 - 2b
           * 7 - c
           * 8 - p
           * 9 - ss
           */
           if (counter == 0)
           {
            if (playerhome)
              batterindex = homebattingindex;
            else
              batterindex = awaybattingindex;

            switch(batterindex)
            {
              case 0:
                batterindex = 8;   
                break;
                
              case 1:
                batterindex = 9;   
                break;

              case 2:
                batterindex = 6;   
                break;

              case 3:
                batterindex = 10;   
                break;

              case 4:
                batterindex = 4;   
                break;

              case 5:
                batterindex = 5;   
                break;

              case 6:
                batterindex = 3;   
                break;

              case 7:
                if (playerhome)
                  batterindex = homepitcher;
                else
                  batterindex = awaypitcher;   
                break;

              case 8:
                batterindex = 7;   
                break;
            }
           }
            
            if (batting == 'h')
            {
              switch(hometeam[batterindex].handed)
              {
                case 0:
                  drawBasher(8,46,0, true, batting);
                  break;
                case 1:
                  drawBasher(48,46,0, false, batting);
                  break;
                case 2:
                  if (awayteam[awaypitcher].handed == 0)
                    drawBasher(48,46,0, false, batting);
                  else
                    drawBasher(8,46,0, true, batting);
                  break;
              }
            }
            else
              {
                switch(awayteam[batterindex].handed)
                {
                  case 0:
                    drawBasher(8,46,0, true, batting);
                    drawPitcher(56,36,0,hometeam[homepitcher].handed,true);
                    break;
                  case 1:
                    drawBasher(56,46,0, false, batting);
                    drawPitcher(8,36,0,hometeam[homepitcher].handed,true);
                    break;
                  case 2:
                    if (hometeam[homepitcher].handed == 0)
                    {
                      drawBasher(56,46,0, false, batting);
                      drawPitcher(8,36,0,hometeam[homepitcher].handed,true);
                    }
                    else
                    {
                      drawBasher(8,46,0, true, batting);
                      drawPitcher(56,36,0,hometeam[homepitcher].handed,true);
                    }
                    break;
                }
              }

            if ((playerhome && batting=='h') || (!playerhome && batting=='a'))
            {
              batbox();

              if (arduboy.pressed(UP_BUTTON) == true && upbuf == 0 && animation == 0)
              {
                upbuf = 1;
                if (batselect <1)
                {
                  batselect = 3;
                }
                else
                {
                  batselect--;
                }
              }
              else if (arduboy.pressed(DOWN_BUTTON) == true && downbuf == 0 && animation == 0) 
              {
                downbuf = 1;
                if (batselect > 2)
                {
                  batselect = 0;
                }
                else
                {
                  batselect++;
                }
              }
  
              
  
              // draw selecter
              switch (batselect)
              {
                case 0:
                  tinyfont.setCursor(81, 6);
                  tinyfont.print("*");
                  break;
                case 1:
                  tinyfont.setCursor(81, 12);
                  tinyfont.print("*");
                  break;
                case 2:
                  tinyfont.setCursor(81, 18);
                  tinyfont.print("*");
                  break;
                case 3:
                  tinyfont.setCursor(81, 24);
                  tinyfont.print("*");
                  break;
              }

              if (arduboy.pressed(A_BUTTON) == true && abuf == 0 && animation == 0)
              {
                abuf = 1;

                animation = 1;
                /*
                field.first = 1;
                field.second = 1;
                field.third = 1;
                batterindex = batselect;
                */
              } 

              
            }
            else
            {
              // pitching
              tinyfont.setCursor(20,20);
              tinyfont.print("Pitching");
            }

            if (animation != 0)
              {
                if (animation == 1)
                {
                  // DETERMINE at bat OUTCOME
                  /*
                       * batselect:
                       * 0: power     -- good vs. fast;     bad vs. break
                       * 1: contact   -- good vs. break;    bad vs. changeup
                       * 2: eye       -- good vs. changeup  bad vs. fast
                       * 
                       * 
                       * pitchselect:
                       * 0: fast      -- good vs. eye;      bad vs. power
                       * 1: break     -- good vs. power;    bad vs. contact
                       * 2: changeup  -- good vs. contact;  bad vs. eye
                       * 
                       * bat attributes:
                       *  power       -- good vs. velocity; bad vs. control
                       *  contact     -- good vs. control;  bad vs. def
                       *  eye         -- good vs. def;      bad vs. velocity
                       *  
                       * pitch attributes:
                       *  velocity    -- good vs. eye;      bad vs. power
                       *  control     -- good vs. power;    bad vs. contact
                       *  def (cu)    -- good vs. contact;  bad vs. eye
                       */
  
                  // determine by pitch/bat selections
                  if ((batselect == 0 && pitchselect == 0) || (batselect == 1 && pitchselect == 1) ||
                    (batselect == 2 && pitchselect == 2))
                  {
                    bashlikelihood[0] += random(0,5);
                  }
                  else if ((pitchselect == 0 && batselect == 2) || (pitchselect == 1 && batselect == 0) ||
                    (pitchselect == 2 && batselect == 1))
                  {
                    bashlikelihood[1] += random(0,5);
                  }
                  else
                  {
                    bashlikelihood[2] += random(0,5);
                  }
                  
                  
                  if (batting == 'h')
                  {
                     // determine by attributes
                    if ((hometeam[batterindex].power > awayteam[awaypitcher].velocity) || (hometeam[batterindex].contact > awayteam[awaypitcher].control) ||
                      (hometeam[batterindex].eye > awayteam[awaypitcher].def))
                    {
                      bashlikelihood[0] += random(0,4);
                    }
                    if ((awayteam[awaypitcher].velocity > hometeam[batterindex].eye) || (awayteam[awaypitcher].control > hometeam[batterindex].power) ||
                      (awayteam[awaypitcher].def > hometeam[batterindex].contact))
                    {
                      bashlikelihood[1] += random(0,4);
                    }
                    if (awayteam[awaypitcher].control < random(3,8))
                    {
                      bashlikelihood[2] += random(0,4);
                    }
  
                    // determine by handedness
                     if ((hometeam[batterindex].handed == 0 && awayteam[awaypitcher].handed == 0) ||
                      (hometeam[batterindex].handed == 1 && awayteam[awaypitcher].handed == 1))
                    {
                      bashlikelihood[1] += random(0,3);
                    }
                  }
                  else
                  {
                    // away team
                    // determine by attributes
                    if ((awayteam[batterindex].power > hometeam[homepitcher].velocity) || (awayteam[batterindex].contact > hometeam[homepitcher].control) ||
                      (awayteam[batterindex].eye > hometeam[homepitcher].def))
                    {
                      bashlikelihood[0] += random(0,4);
                    }
                    if ((hometeam[homepitcher].velocity > awayteam[batterindex].eye) || (hometeam[homepitcher].control > awayteam[batterindex].power) ||
                      (hometeam[homepitcher].def > awayteam[batterindex].contact))
                    {
                      bashlikelihood[1] += random(0,4);
                    }
                    if (hometeam[homepitcher].control < random (3,8))
                    {
                      bashlikelihood[2] += random(0,4);
                    }
  
                    // determine by handedness
                    if ((awayteam[batterindex].handed == 0 && hometeam[homepitcher].handed == 0) ||
                      (awayteam[batterindex].handed == 1 && hometeam[homepitcher].handed == 1))
                    {
                      bashlikelihood[1] += random(0,3);;
                    }
                  }
  
                  // set OUTCOME
                  if ((bashlikelihood[0] > bashlikelihood[1]) && (bashlikelihood[0] > bashlikelihood[2]))
                  {
                    bashlikelihood[3] = 0;
                  }
                  else if ((bashlikelihood[1] > bashlikelihood[0]) && (bashlikelihood[1] > bashlikelihood[2]))
                  {
                    bashlikelihood[3] = 1;
                  }
                  else
                  {
                    bashlikelihood[3] = 2;
                  }
                }

                arduboy.setCursor(0,0);
                arduboy.print(bashlikelihood[3]);
                
                // ANIMATE pitch    
                if (batting == 'h')
                {
                  switch(hometeam[batterindex].handed)
                  {
                    case 0:
                      drawBasher(8,46,0, true, batting);
                      break;
                    case 1:
                      drawBasher(48,46,0, false, batting);
                      break;
                    case 2:
                      if (awayteam[awaypitcher].handed == 0)
                        drawBasher(48,46,0, false, batting);
                      else
                        drawBasher(8,46,0, true, batting);
                      break;
                  }
                }
                else
                {
                // away team batting
                  if (animation == 1)
                  {
                    
                    if (playerhome)
                    {
                      batselect = random(0,3);  
                    }
                    else
                    {
                      pitchselect = random(0,3);
                    }
                  }

                  //arduboy.setCursor(1,1);
                  //arduboy.print(pitchselect);

                  
                  
                  
                  if (awayteam[batterindex].handed == 0 || (awayteam[batterindex].handed == 2 && hometeam[homepitcher].handed != 0))
                  {
                    pitchselect = 0;
                    switch(pitchselect)
                    {
                        case 0:
                          if (animation < 2)
                          {
                            arduboy.drawPixel(52,40); 
                          }
                          else if (animation < 4)
                          {
                            arduboy.drawPixel(51,40);
                          }
                          else if (animation < 6)
                          {
                            arduboy.drawPixel(50,41);
                          }
                          else if (animation < 8)
                          {
                            arduboy.drawPixel(49,41);
                          }
                          else if (animation < 9)
                          {
                            arduboy.drawPixel(48,42);
                          }
                          else if (animation < 10)
                          {
                            arduboy.drawPixel(47,42);
                          }
                          else if (animation < 11)
                          {
                            arduboy.fillCircle(46,43,2);
                          }
                          else if (animation < 12)
                          {
                            arduboy.fillCircle(45,43,2);
                          }
                          break;

                        case 1:

                          break;
                    }
                  }
                  else
                  {
                    arduboy.setCursor(1,1);
                    if (animation < 2)
                    {
                      arduboy.drawPixel(14,40); 
                    }
                    else if (animation < 4);
                    {
                      arduboy.drawPixel(15,41);
                    }
                  }
                   
                }
        
    

                  animation++;
              }

            // count
            tinyfont.setCursor(80, 37);
            tinyfont.print(bashball);
            tinyfont.print("-");
            tinyfont.print(bashstrike);
            tinyfont.print(",OUT:");
            tinyfont.print(bashout); 


            tinyfont.setCursor(80,44);
            tinyfont.print("PITCHR#");
            if (batting == 'a')
            {
              if (hometeam[homepitcher].number < 10)
                tinyfont.print("0");
              tinyfont.print(hometeam[homepitcher].number);
            }
            else
            {
              if (awayteam[awaypitcher].number < 10)
                tinyfont.print("0");
              tinyfont.print(awayteam[awaypitcher].number);
            }

            tinyfont.setCursor(80, 51);
            tinyfont.print("BATTER#");
            if (batting == 'h')
            {
              if (hometeam[batterindex].number < 10)
                tinyfont.print("0");
              tinyfont.print(hometeam[batterindex].number);
            }
            else
            {
              if (awayteam[batterindex].number < 10)
                tinyfont.print("0");
              tinyfont.print(awayteam[batterindex].number);
            }
            
            tinyfont.setCursor(80, 58);
            tinyfont.print("B--F/INFO"); 
            
            drawField(12,4);
            
            if (arduboy.pressed(B_BUTTON) == true && bbuf == 0)
            {
              bbuf = 1;
              changestate(playstate, 4);
            }
       
            counter++;
            break;

           case 3:
            // pitching

            break;

          case 4:
            //info
            tinyfont.setCursor(4,4);
            if (infoshowhome)
              tinyfont.print("HOME");
            else
              tinyfont.print("AWAY");

            tinyfont.setCursor(40, 4);
            tinyfont.print("A -- Switch Teams");
            for (int i = infoindex; i < (infoindex + 7); i++) 
            {
              tinyfont.setCursor(4, 8*(i-infoindex)+12);

              tinyfont.print("#");
              if (infoshowhome)
              {
                if (hometeam[i].number < 10)
                  tinyfont.print("0");
                tinyfont.print(hometeam[i].number);

                // print position
                tinyfont.print("\t");
                switch(hometeam[i].pos)
                {
                  case 0:
                    tinyfont.print("Ca");
                    break;

                  case 1:
                    tinyfont.print("1B");
                    break;

                  case 2:
                    tinyfont.print("2B");
                    break;

                  case 3:
                    tinyfont.print("3B");
                    break;

                  case 4:
                    tinyfont.print("SS");
                    break;

                  case 5:
                    tinyfont.print("LF");
                    break;

                  case 6:
                    tinyfont.print("CF");
                    break;

                  case 7:
                    tinyfont.print("RF");
                    break;

                  case 8:
                    tinyfont.print("SP");
                    break;

                  case 9:
                    tinyfont.print("RP");
                    break;
                }
                
                // print handed
                tinyfont.print("\t");
                switch (hometeam[i].handed)
                {
                  case 0:
                    tinyfont.print("R");
                    break;
                   case 1:
                    tinyfont.print("L");
                    break;
                   case 2:
                    tinyfont.print("S");
                    break;
                }

                // print attributes
                tinyfont.print("\t");
                // check if pitcher
                if (hometeam[i].pos == 8 || hometeam[i].pos == 9)
                {
                  tinyfont.print("*");
                  tinyfont.print(hometeam[i].velocity);

                  tinyfont.print("\t");
                  tinyfont.print("*");
                  tinyfont.print(hometeam[i].control);

                  tinyfont.print("\t");
                  tinyfont.print("*");
                  tinyfont.print(hometeam[i].def);
                }
                else
                {
                  tinyfont.print(" ");
                  tinyfont.print(hometeam[i].power);

                  tinyfont.print("\t");
                  tinyfont.print(" ");
                  tinyfont.print(hometeam[i].contact);

                  tinyfont.print("\t");
                  tinyfont.print(" ");
                  tinyfont.print(hometeam[i].eye);
                }               
              }
              else
              {
                if (awayteam[i].number < 10)
                  tinyfont.print("0");
                tinyfont.print(awayteam[i].number);

                

                // print position
                tinyfont.print("\t");
                switch(awayteam[i].pos)
                {
                  case 0:
                    tinyfont.print("Ca");
                    break;

                  case 1:
                    tinyfont.print("1B");
                    break;

                  case 2:
                    tinyfont.print("2B");
                    break;

                  case 3:
                    tinyfont.print("3B");
                    break;

                  case 4:
                    tinyfont.print("SS");
                    break;

                  case 5:
                    tinyfont.print("LF");
                    break;

                  case 6:
                    tinyfont.print("CF");
                    break;

                  case 7:
                    tinyfont.print("RF");
                    break;

                  case 8:
                    tinyfont.print("SP");
                    break;

                  case 9:
                    tinyfont.print("RP");
                    break;
                }

                // print handed
                tinyfont.print("\t");
                switch (awayteam[i].handed)
                {
                  case 0:
                    tinyfont.print("R");
                    break;
                   case 1:
                    tinyfont.print("L");
                    break;
                   case 2:
                    tinyfont.print("S");
                    break;
                }

                // print attributes
                tinyfont.print("\t");
                // check if pitcher
                if (awayteam[i].pos == 8 || awayteam[i].pos == 9)
                {
                  tinyfont.print("*");
                  tinyfont.print(awayteam[i].velocity);

                  tinyfont.print("\t");
                  tinyfont.print("*");
                  tinyfont.print(awayteam[i].control);

                  tinyfont.print("\t");
                  tinyfont.print("*");
                  tinyfont.print(awayteam[i].def);
                }
                else
                {
                  tinyfont.print(" ");
                  tinyfont.print(awayteam[i].power);

                  tinyfont.print("\t");
                  tinyfont.print(" ");
                  tinyfont.print(awayteam[i].contact);

                  tinyfont.print("\t");
                  tinyfont.print(" ");
                  tinyfont.print(awayteam[i].eye);
                }
              }

              arduboy.drawLine(2, 8*(i-infoindex)+17, 120, 8*(i-infoindex)+17);
            }

            if (arduboy.pressed(A_BUTTON) == true && abuf == 0)
            {
              abuf = 1;
              if (infoshowhome)
                infoshowhome = false;
              else
                infoshowhome = true;
            }
            else if (arduboy.pressed(B_BUTTON) == true && bbuf == 0)
            {
              bbuf = 1;
              playstate = 2;
              //changestate(playstate, laststate);
            }

            if (arduboy.pressed(UP_BUTTON) == true && upbuf == 0)
            {
              upbuf = 1;
              if (infoindex > 0)
              {
                infoindex--;
              }
            }
            else if (arduboy.pressed(DOWN_BUTTON) == true && downbuf == 0)
            {
              downbuf = 1;
              if (infoindex < 4)
              {
                infoindex++;
              }
            }

            switch (infoindex)
            {
              case 0:
                arduboy.drawLine(127, 16, 127, 24);
                break;

              case 1:
                arduboy.drawLine(127, 24, 127, 32);
                break;

              case 2:
                arduboy.drawLine(127, 32, 127, 40);
                break;

              case 3:
                arduboy.drawLine(127, 40, 127, 48);
                break;

              case 4:
                arduboy.drawLine(127, 48, 127, 56);
                break;
            }
            break;
        }
       
       
        
      break;

    case 2:
      // win

      break;

    case 3:
      // lose

      break;
  }

  if( arduboy.notPressed(UP_BUTTON) == true && arduboy.notPressed(LEFT_BUTTON) == true) {
    upbuf = 0;
  }
  if( arduboy.notPressed(DOWN_BUTTON) == true && arduboy.notPressed(RIGHT_BUTTON) == true) {
    downbuf = 0;
  }
  if( arduboy.notPressed(A_BUTTON) == true ) {
    abuf = 0;
  }
  if( arduboy.notPressed(B_BUTTON) == true ) {
    bbuf = 0;
  }
  

        
  
  arduboy.display();
}
