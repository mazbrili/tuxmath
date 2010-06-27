/*
  schoolmode.c

  For TuxMath
  Contains code for implementing school mode in Tuxmath.

  by Vikas Singh
  vikassingh008@gmail.com 


  Part of "Tux4Kids" Project
  http://www.tux4kids.com/

 
*/

#include "menu.h"
#include "SDL_extras.h"
#include "titlescreen.h"
#include "highscore.h"
#include "factoroids.h"
#include "credits.h"
#include "multiplayer.h"
#include "mathcards.h"
#include "campaign.h"
#include "game.h"
#include "options.h"
#include "fileops.h"
#include "setup.h"
#include "throttle.h"
#include"compiler.h"
//#include "schoolmode.h"
#include"parse_xmlLesson.h"
#include "globals.h"
#include "loaders.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//levels displayed on screen at a time
#define MENUS_ON_SCREEN 6   
SDL_Surface *tux4kids_logo=NULL;
SDL_Rect logo_rect1,tux4kids_logo_rect;
SDL_Rect bkgd_rect,rects;

SDL_Surface* win_bkgd = NULL;
SDL_Surface* fs_bkgd= NULL;

SDL_Rect stop_rect, prev_rect, next_rect;
SDL_Surface *stop_button, *prev_arrow, *next_arrow, *prev_gray, *next_gray;



const char* tux4kids_standby_path = "status/standby.svg";
//const char* school_bkg_path    = "schoolmode/school_bkg.jpg";
const char* school_bkg_path="backgrounds/6.jpg";
enum { NONE, CLICK, PAGEUP, PAGEDOWN, STOP_ESC, RESIZED };


SDL_Surface* current_bkgd()
  { return screen->flags & SDL_FULLSCREEN ? fs_bkgd : win_bkgd; }


/* Local function prototypes: */
void ShowMsg(char*,char*,char*,char*);


int total_no_menus; //defined in parse_xmlLesson.h
int no_of_pages;        //no of menu pages to be displayed



void sm_prerender_all()
{
const float stop_pos[4] = {0.94, 0.0, 0.06, 0.06};
const float prev_pos[4] = {0.87, 0.93, 0.06, 0.06};
const float next_pos[4] = {0.94, 0.93, 0.06, 0.06};
const char* stop_path = "status/stop.svg";
const char* prev_path = "status/left.svg";
const char* next_path = "status/right.svg";
const char* prev_gray_path = "status/left_gray.svg";
const char* next_gray_path = "status/right_gray.svg";

  SetRect(&stop_rect, stop_pos);
  if(stop_button)
    SDL_FreeSurface(stop_button);
  stop_button = LoadImageOfBoundingBox(stop_path, IMG_ALPHA, stop_rect.w, stop_rect.h);
  /* move button to the right */
  stop_rect.x = screen->w - stop_button->w;

  SetRect(&prev_rect, prev_pos);
  if(prev_arrow)
    SDL_FreeSurface(prev_arrow);
  prev_arrow = LoadImageOfBoundingBox(prev_path, IMG_ALPHA, prev_rect.w, prev_rect.h);
  if(prev_gray)
    SDL_FreeSurface(prev_gray);
  prev_gray = LoadImageOfBoundingBox(prev_gray_path, IMG_ALPHA, prev_rect.w, prev_rect.h);
  /* move button to the right */
  prev_rect.x += prev_rect.w - prev_arrow->w;

  SetRect(&next_rect, next_pos);
  if(next_arrow)
    SDL_FreeSurface(next_arrow);
  next_arrow = LoadImageOfBoundingBox(next_path, IMG_ALPHA, next_rect.w, next_rect.h);
  if(next_gray)
    SDL_FreeSurface(next_gray);
  next_gray = LoadImageOfBoundingBox(next_gray_path, IMG_ALPHA, next_rect.w, next_rect.h);

 
}









int schoolmode(void)
{

  tux4kids_logo = LoadImage(tux4kids_standby_path, IMG_REGULAR);

  /* display the Standby screen */
  if(tux4kids_logo)
  {
    /*Center horizontally and vertically */
    tux4kids_logo_rect.x = (screen->w - tux4kids_logo->w) / 2;
    tux4kids_logo_rect.y = (screen->h - tux4kids_logo->h) / 2;

    tux4kids_logo_rect.w = tux4kids_logo->w;
    tux4kids_logo_rect.h = tux4kids_logo->h;

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_BlitSurface(tux4kids_logo, NULL, screen, &tux4kids_logo_rect);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    /* Play "harp" greeting sound lifted from Tux Paint */
    playsound(SND_HARP);
    SDL_FreeSurface(tux4kids_logo);
  }

sm_prerender_all(); //write function declaration too   -- fix

SDL_Delay(2000);

 //LoadBothBkgds(school_bkg_path, &fs_bkgd, &win_bkgd);

//display_screen();
parse_xmlLesson();


return 0;
}


void display_screen(int selected)
{

int temp,current_no;
int i = 0;
  Uint32 timer = 0;

  int finished = 0;
  Uint32 frame = 0;
  Uint32 start = 0;
 SDL_Surface* srfc = NULL;
        SDL_Rect text_rect, button_rect,dest,table_bg;

  int page_no = 0;
  int old_page_no = -1; //So menus get drawn first time through
  /* Surfaces, char buffers, and rects for table: */
  
 

  const int max_width = 500;

  const int title_font_size = 32;
 

//decide the no of menu pages to be displayed
if(total_no_menus % MENUS_ON_SCREEN)
  no_of_pages=(total_no_menus/MENUS_ON_SCREEN)+1;
else
 no_of_pages=total_no_menus/MENUS_ON_SCREEN;
  



  LoadBothBkgds(school_bkg_path, &fs_bkgd, &win_bkgd);


bkgd_rect = current_bkgd()->clip_rect;
    bkgd_rect.x = (screen->w - bkgd_rect.w) / 2;
    bkgd_rect.y = (screen->h - bkgd_rect.h) / 2;



if(current_bkgd())
  {
    /* FIXME not sure trans_wipe() works in Windows: */
    trans_wipe(current_bkgd(), RANDOM_WIPE, 10, 20);
    /* Make sure background gets drawn (since trans_wipe() doesn't */
    /* seem to work reliably as of yet):                          */
    SDL_BlitSurface(current_bkgd(), NULL, screen, &bkgd_rect);
  }

      /* Draw background shading for table: */
      table_bg.x = (screen->w)/2 - (max_width + 20)/2 + 50; //don't draw over Tux
      table_bg.y = 5;
      table_bg.w = max_width + 20;
      table_bg.h = screen->h - 100; //- images[IMG_RIGHT]->h;
      DrawButton(&table_bg, 25, SEL_RGBA);


  while (!finished)
  {
    start = SDL_GetTicks();

    /* Check for user events: */
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          cleanup();
        }

        case SDL_MOUSEBUTTONDOWN:
        /* "Stop" button - go to main menu: */
        {
          if (inRect(stop_rect, event.button.x, event.button.y ))
          {
            finished = 1;
            playsound(SND_TOCK);
          }

          /* "Left" button - go to previous page: */
          if (inRect(prev_rect, event.button.x, event.button.y))
          {
            if (page_no > 0)
            {
              page_no--;
              if (Opts_GetGlobalOpt(MENU_SOUND))
              {
                playsound(SND_TOCK);
              }
            }
          }

          /* "Right" button - go to next page: */
          if (inRect(next_rect, event.button.x, event.button.y ))
          {
            if (page_no < no_of_pages-1)
            {
              page_no++;
              if (Opts_GetGlobalOpt(MENU_SOUND))
              {
                playsound(SND_TOCK);
              }
            }
          }
          break;
        }


        case SDL_KEYDOWN:
        {
          if (event.key.keysym.sym == SDLK_RETURN)   
           { 
            finished = 1;
            playsound(SND_TOCK);
           } 
        }
      }
          Throttle(20, &timer);
    }


    /* If needed, redraw: */
    if (page_no != old_page_no)
    {

       
    //  DrawTitleScreen();
      /* Draw controls: */
      if (stop_button)
        SDL_BlitSurface(stop_button, NULL, screen, &stop_rect);
      
      /* Draw regular or grayed-out left arrow: */
      if (page_no == 0)
      {
        if (prev_gray)
          SDL_BlitSurface(prev_gray, NULL, screen, &prev_rect);
      }
      else
      {
        if (prev_arrow)
          SDL_BlitSurface(prev_arrow, NULL, screen, &prev_rect);
      }
      /* Draw regular or grayed-out right arrow: */
      if (page_no == no_of_pages-1)
      {
        if (next_gray)
          SDL_BlitSurface(next_gray, NULL, screen, &next_rect);
      }
      else
      {
        if (next_arrow)
          SDL_BlitSurface(next_arrow, NULL, screen, &next_rect);
      }



 temp=page_no*MENUS_ON_SCREEN; //menu display begins from temp at this page

    srfc = BlackOutline(_("MISSIONS "), 65, &white);
        if (srfc)
        {
          button_rect.x = text_rect.x = (screen->w)/2 - (srfc->w)/2 + 50;
          button_rect.y = text_rect.y = 10;
          button_rect.w = text_rect.w = srfc->w;
          button_rect.h = text_rect.h = srfc->h+5;
          /* add margin to button and draw: */
          button_rect.x -= 10;
          button_rect.w += 20;
          DrawButton(&button_rect, 15, 0, 0, 32, 192);
          /* Now blit text and free surface: */
          SDL_BlitSurface(srfc, NULL, screen, &text_rect);
          SDL_FreeSurface(srfc);
          srfc = NULL;
        }

current_no=total_no_menus-temp;


current_no=min(MENUS_ON_SCREEN,current_no);

for(i=0;i<current_no;i++)
{
   if(temp+i==selected)   
    srfc = BlackOutline(_(menu_names[temp+i]),55 , &yellow);   //title_font_size
   else   
    srfc = BlackOutline(_(menu_names[temp+i]),55 , &white); 
        if (srfc)
        {
          text_rect.x = (screen->w)/2 - (srfc->w)/2 + 50;
          text_rect.y += text_rect.h; /* go to bottom of next line */
          text_rect.w = srfc->w;
          text_rect.h = srfc->h;
          SDL_BlitSurface(srfc, NULL, screen, &text_rect);
          SDL_FreeSurface(srfc);
          srfc = NULL;
         if(temp+i < selected) 
         {
          dest.x = text_rect.x  + max_width -140;
          dest.y = text_rect.y ;
          dest.w = images[IMG_CORRECT]->w;
          dest.h = images[IMG_CORRECT]->h;
 
          SDL_BlitSurface(images[IMG_CORRECT], NULL, screen, &dest);    
         }  
        }
      }


}






      SDL_UpdateRect(screen, 0, 0, 0, 0);

      old_page_no = page_no;
    

  //  HandleTitleScreenAnimations();

    /* Wait so we keep frame rate constant: */
    while ((SDL_GetTicks() - start) < 33)
    {
      SDL_Delay(20);
    }
    frame++;

  }  // End of while (!finished) loop
}




void ShowMsg(char* str1, char* str2, char* str3, char* str4)
{
  SDL_Surface *s1, *s2, *s3, *s4;
  SDL_Rect loc;

  s1 = s2 = s3 = s4 = NULL;
 if (str1)
    s1 = BlackOutline(str1, 40, &white);
  if (str2)
    s2 = BlackOutline(str2, 40, &white);
  if (str3)
    s3 = BlackOutline(str3, 25, &white);
  /* When we get going with i18n may need to modify following - see below: */
  if (str4)
    s4 = BlackOutline(str4, 25, &white);
  /* Draw lines of text (do after drawing Tux so text is in front): */
  if (s1)
  {
    loc.x = (screen->w / 6) - (s1->w/2); 
    loc.y = (screen->h / 12)-30 ;
    SDL_BlitSurface( s1, NULL, screen, &loc);
  }
  if (s2)
  {
    loc.x = (screen->w/2 ) + (s2->w); 
    loc.y = (screen->h / 12) - 30;
    SDL_BlitSurface( s2, NULL, screen, &loc);
  }
  if (s3)
  {
    loc.x = (screen->w / 4) ; 
    loc.y = (screen->h / 3)+30 ;
    SDL_BlitSurface( s3, NULL, screen, &loc);
  }
  if (s4)
  {
    loc.x = (screen->w / 6) - (s4->w/2); 
    loc.y = (screen->h ) - 100;
    SDL_BlitSurface( s4, NULL, screen, &loc);
  }

  /* and update: */
  SDL_UpdateRect(screen, 0, 0, 0, 0);


  SDL_FreeSurface(s1);
  SDL_FreeSurface(s2);
  SDL_FreeSurface(s3);
  SDL_FreeSurface(s4);
}



