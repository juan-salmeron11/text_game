
#include "neslib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"
//#link "text_title.s"

extern const byte text_title_pal[16];
extern const byte text_title_rle[];

char a[1], b[2];
char i;
char pad;
  
int temp1, temp2, tempResult; //For temporary holding random ints and result after ops
char num1[2], num2[2], result[2];	//Random numbers will be turned to string and placed here
int operator = 0; //If 0 ADD, if 1 MULTIPLY
char dummyResult[2]; //Stores all the wrong answers
  
int y =  9;
int row = 9; //row arrow is at
int ans = 1;// correct answer choice
int choice = 1;// player choice
int correct = 0;// number of questions correct
int timer = 450;// timer for score
int question = 0; //current question
int score = 0;
int f; //for frames
int ti = 0;

byte rndint(byte a, byte b);
void set_question(void);
void controls(void);
void title(void);
void layout(void);
void score_screen(void);



void fade_in() {
  byte vb;
  for (vb=0; vb<=4; vb++) {
    // set virtual bright value
    pal_bright(vb);
    // wait for 4/60 sec
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
}

void show_title_screen(const byte* pal, const byte* rle) {
  // disable rendering
  ppu_off();
  // set palette, virtual bright to 0 (total black)
  pal_bg(pal);
  pal_bright(0);
  // unpack nametable into the VRAM
  vram_adr(0x2000);
  vram_unrle(rle);
  // enable rendering
  ppu_on_all();
  // fade in from black
  fade_in();
}




void main(void) {
  title();
  
  // set palette colors
  pal_col(0,0x28);	// screen 
  pal_col(1,0x11);	// 
  pal_col(2,0x11);	// 
  pal_col(3,0x30);	// 
  
  
  ppu_off();
  layout();
  set_question();
  
  ppu_on_all();
  // Question Loop
  while (timer > 0)
  {
    
    controls();
    ppu_wait_frame();  	//work in progress for timer
    timer--;		// decrease by 1 every frame
    
    if(timer % 50 == 0)
    {
      ppu_off();
      y -=1;
      sprintf(a, "%d", y);
      vram_adr(NTADR_A(28,2));		// set address
      vram_write(a, 1);			// write bytes to video RAM
      ppu_on_all();
    }
    
    if(ti == 0){
      ti = 1;
      ppu_off();
      for(f=0; f < 10; f++) ppu_wait_frame();
      //clear and load next question
      set_question();
     // y = 9;
     // sprintf(a, "%d", y);
      vram_adr(NTADR_A(28,2));		// set address
      vram_write(a, 1);			// write bytes to video RAM
      question++;
      ppu_on_all();
    }
  };
  
  score_screen();
  
  while(1){};
  
};



void score_screen(){
  ppu_off();
  y = score;
  sprintf(b, "%d", y);
  vram_adr(NAMETABLE_A);
  vram_fill(0,1024);  
  vram_adr(NTADR_A(7,14)); 
  vram_write("You Answered:", 14);
  vram_adr(NTADR_A(20,14));		// set address
  vram_write(b, 2);			

  ppu_on_all();

}

void layout(){
  
  vram_adr(NAMETABLE_A);
  vram_fill(0,1024);
  vram_adr(NTADR_A(3,2)); 
  vram_write("What is ....", 11);
  vram_adr(NTADR_A(17,2));
  vram_write("?", 1);
  
  
  sprintf(a, "%d", y);
  vram_adr(NTADR_A(28,2)); 	//set timer
  vram_write(a, 1);	
  vram_adr(NTADR_A(3,9));
  vram_write("\x1f", 1);	//set arrow to first choice on row 9
  
  vram_adr(NTADR_A(5,9));
  vram_write("(A)",3);
  vram_adr(NTADR_A(5,11));		
  vram_write("(B)",3);	
  vram_adr(NTADR_A(5,13));		
  vram_write("(C)",3);
  vram_adr(NTADR_A(5,15));		
  vram_write("(D)",3); 
}

void title(){ 
  
   show_title_screen(text_title_pal, text_title_rle);
   while(1){
   pad = pad_trigger(i);
   if(pad & PAD_START)
   {
      ppu_off();
      for(f=0; f < 20; f++) ppu_wait_frame();
      break;
    }
  temp1 = rndint(1,9);
  temp2 = rndint(1,9);
  operator = rndint(1,90);
  };
  
}


void controls(){
      pad = pad_trigger(i);
  //player pressed down and arrow is not on last choice
    if (pad & PAD_DOWN && row < 14 && choice < 4) {
      	ppu_off();
      	vram_adr(NTADR_A(3,row));
      	vram_write(NULL, 1);	//erase arrow at previous choice	
      	row += 2;			
     	vram_adr(NTADR_A(3,row)); //increment row and set arrow on next choice
  	vram_write("\x1f", 1);	
      	
      	
      	choice++;	// increment player choice
       ppu_on_all();                
    }
    
    //player presses up and arrow isn not on first choice
    if (pad & PAD_UP && row > 9 && choice > 1) {
      	ppu_off();
      	vram_adr(NTADR_A(3,row));
      	vram_write(NULL, 1);
      	row -= 2;
      	vram_adr(NTADR_A(3,row));
  	vram_write("\x1f", 1);
      	
      	choice--;
      	ppu_on_all();

                          }
    if( pad & PAD_A )
    {
      if(ans == choice) //if answer is correct flash green and increment num of correct
      {
        pal_col(0,0x19);
  	for(f = 0; f < 8; f++) ppu_wait_frame();
        pal_col(0,0x28);
        correct++;
        score += 1; 
        ti = 0;
      }
     
      else		// else flash red
      {
        pal_col(0,0x16);
        for(f = 0; f < 8; f++) ppu_wait_frame();
        pal_col(0,0x28);
       ti = 0;
      } 
    }
    
  
}

void set_question(){
  
  temp1 = rndint(1,9);
  temp2 = rndint(1,9);
  operator = rndint(1,90);
  
  sprintf(num1, "%d", temp1); 
  sprintf(num2, "%d", temp2);
  
    //Display operands and operators
  vram_adr(NTADR_A(11,2));		
  vram_write(num1, 2);
  vram_adr(NTADR_A(15,2));		
  vram_write(num2, 2);	
  
  //If operator is 0, add the operands
  if (operator % 2 == 0){
    vram_adr(NTADR_A(13,2));
    vram_write("+", 2);
    tempResult = temp1 +temp2;
    sprintf(result, "%d", temp1 + temp2);
  // If operator is 1, multiply operands  
  }else{
    vram_adr(NTADR_A(13,2));
    vram_write("x", 2);
     tempResult = temp1 * temp2;
    sprintf(result, "%d", temp1 * temp2);
  }
  
   ans = rndint(1,4);
  
  
  switch(ans) {

   case 1:
  	vram_adr(NTADR_A(9,9));		
  	vram_write(result,2);		//Correct asn
  	
      	sprintf(dummyResult, "%d", tempResult + 1);
      
  	vram_adr(NTADR_A(9,11));		
  	vram_write(dummyResult,2);
      
      	sprintf(dummyResult, "%d", tempResult + 2);
      
  	vram_adr(NTADR_A(9,13));		
  	vram_write(dummyResult,2);
  
      	sprintf(dummyResult, "%d", tempResult - 1);
      
  	vram_adr(NTADR_A(9,15));		
  	vram_write(dummyResult,2);
      break; 
	
    case 2:
      	sprintf(dummyResult, "%d", tempResult + 1);
      
  	vram_adr(NTADR_A(9,9));		
  	vram_write(dummyResult,2);

  	vram_adr(NTADR_A(9,11));		
  	vram_write(result,2);		//Correct Ans
  	
      	sprintf(dummyResult, "%d", tempResult + 2);
      
  	vram_adr(NTADR_A(9,13));		
  	vram_write(dummyResult,2);
  	
      	sprintf(dummyResult, "%d", tempResult - 1);
      
  	vram_adr(NTADR_A(9,15));		
  	vram_write(dummyResult,2);      
      break; 
    case 3:
      	sprintf(dummyResult, "%d", tempResult + 1);
      
  	vram_adr(NTADR_A(9,9));		
  	vram_write(dummyResult,2);

      	sprintf(dummyResult, "%d", tempResult + 2);
      
  	vram_adr(NTADR_A(9,11));		
  	vram_write(dummyResult,2);
 
  	vram_adr(NTADR_A(9,13));		
  	vram_write(result,2);		//Correct answer

      	sprintf(dummyResult, "%d", tempResult - 1);
      
  	vram_adr(NTADR_A(9,15));		
  	vram_write(dummyResult,2);      
      break; 
	
    case 4:
      	sprintf(dummyResult, "%d", tempResult + 1);
      
  	vram_adr(NTADR_A(9,9));		
  	vram_write(dummyResult,2);

      	sprintf(dummyResult, "%d", tempResult + 2);
      
  	vram_adr(NTADR_A(9,11));		
  	vram_write(dummyResult,2);

      	sprintf(dummyResult, "%d", tempResult - 1);	
      
  	vram_adr(NTADR_A(9,13));		
  	vram_write(dummyResult,2);

  	vram_adr(NTADR_A(9,15));		
  	vram_write(result,2);      //Correct Ans
      break; 
  
   default : 
      break;
}
}

byte rndint(byte a, byte b){
  return (rand() % (b-a)) + a;
}
