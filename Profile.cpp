#include <gtkmm.h>
#include <libglademm/xml.h>
#include <gtkglmm.h>
#include <vector>
#include <iostream>
#include "quadtree.h"
#include "quadtreestructs.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Profile.h"
#include "MathFuncs.h"

Profile::Profile(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel)  : Gtk::GL::DrawingArea(config){
   this->lidardata=lidardata;
   this->bucketlimit = bucketlimit;
   zoompower = 0.7;
   imageexists=false;
   maindetailmod = 0;
   previewdetailmod = 0.3;
   pointsize=1;
   drawpoints = true;
   drawmovingaverage = false;
   mavrgrange = 5;
   //Initial state:
   lidarboundary = lidardata->getboundary();
   double xdif = lidarboundary->maxX-lidarboundary->minX;
   double ydif = lidarboundary->maxY-lidarboundary->minY;
      //Initial centre:
      viewerx = centrex = lidarboundary->minX+xdif/2;
      viewery = centrey = lidarboundary->minY+ydif/2;
      viewerz = centrez = 0;
      //Scaling to screen dimensions:
      double swidth = get_screen()->get_width();
      double sheight = get_screen()->get_height();
      double xratio = xdif/swidth;
      double yratio = ydif/sheight;
      yratio*=1.25;
      ratio = 0;
      if(xratio>yratio)ratio = xratio;
      else ratio = yratio;
      zoomlevel=1;
   //Rulering:
   rulering=false;
   rulerwidth=2;
   this->rulerlabel = rulerlabel;
   //Colouring and shading:
   heightcolour = false;
   heightbrightness = false;
   zoffset=0;
   zfloor=0.25;
   intensitycolour = false;
   intensitybrightness = true;
   intensityoffset = 0.0/3;
   intensityfloor = 0;
   rmaxz=rminz=0;
   rmaxintensity=rminintensity=0;
   linecolour = true;
   classcolour = false;
   returncolour = false;
   colourheightarray = new double[2];
   colourintensityarray = new double[2];
   brightnessheightarray = new double[2];
   brightnessintensityarray = new double[2];
   buckets = new pointbucket*[2];
   correctpointsbuckets = new bool*[2];
   correctpointsbuckets[0] = new bool[2];
   correctpointsbuckets[1] = new bool[2];
   numbuckets = 2;
   //Events and signals:
   add_events(Gdk::SCROLL_MASK   |   Gdk::BUTTON1_MOTION_MASK   |   Gdk::BUTTON_PRESS_MASK   |   Gdk::BUTTON_RELEASE_MASK);
   signal_scroll_event().connect(sigc::mem_fun(*this,&Profile::on_zoom));
   sigpanstart = signal_button_press_event().connect(sigc::mem_fun(*this,&Profile::on_pan_start));
   sigpan = signal_motion_notify_event().connect(sigc::mem_fun(*this,&Profile::on_pan));
   sigpanend = signal_button_release_event().connect(sigc::mem_fun(*this,&Profile::on_pan_end));
   sigrulerstart = signal_button_press_event().connect(sigc::mem_fun(*this,&Profile::on_ruler_start));
   sigruler = signal_motion_notify_event().connect(sigc::mem_fun(*this,&Profile::on_ruler));
   sigrulerend = signal_button_release_event().connect(sigc::mem_fun(*this,&Profile::on_ruler_end));
   sigrulerstart.block();
   sigruler.block();
   sigrulerend.block();
}

Profile::~Profile(){
   delete[] colourheightarray;
   delete[] colourintensityarray;
   delete[] brightnessheightarray;
   delete[] brightnessintensityarray;
//   delete lidarboundary;
}

bool Profile::returntostart(){
  centrex = startx;
  centrey = starty;
  centrez = rminz + (rmaxz - rminz)/2;
  zoomlevel=1;
  resetview();
  return drawviewable(1);
}

//This method accepts the parameters of the profile and gets the data from the quadtree. It then determines which points from the returned buckets are actually within the boundaries of the profile and then draws them by calling drawviewable(1).
bool Profile::showprofile(double startx,double starty,double endx,double endy,double width){
  this->startx = startx;
  this->starty = starty;
  this->endx = endx;
  this->endy = endy;
  this->width = width;
  centrex = startx;
  centrey = starty;
  centrez = rminz + (rmaxz - rminz)/2;
  for(int i=0;i<numbuckets;i++)delete[] correctpointsbuckets[i];
  delete[] correctpointsbuckets;
  delete[] buckets;
  vector<pointbucket*> *pointvector;
  try{
     pointvector = lidardata->advsubset(startx,starty,endx,endy,width);//Get data.
     imageexists=true;
  }catch(const char* e){
     cout << e << endl;
     cout << "No points returned." << endl;
     imageexists=false;
     return false;
  }
  numbuckets = pointvector->size();
  buckets = new pointbucket*[numbuckets];
  correctpointsbuckets = new bool*[numbuckets];
  for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
     buckets[i]=pointvector->at(i);
     correctpointsbuckets[i] = vetpoints(buckets[i]->numberofpoints,buckets[i]->points,startx,starty,endx,endy,width);
  }
  resetview();
  delete pointvector;
  return drawviewable(1);
}

//Depending on the imagetype requested, this sets the detail level and then calls one of the image methods, which actually draws the data to the screen.
bool Profile::drawviewable(int imagetype){
   if(!imageexists){//If there is an attempt to draw with no data, the program will probably crash.
      Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
      if (!glwindow->gl_begin(get_gl_context()))return false;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
      if (glwindow->is_double_buffered())glwindow->swap_buffers();
      else glFlush();
      return false;
   }
   glViewport(0, 0, get_width(), get_height());//THIS IS A HACK! This is in order to temporarily make the program work with multiple windows. Hopefully there is a better way.
   int detail=1;//This determines how many points are skipped between reads.
   if(imagetype==1){
      detail=(int)(numbuckets*maindetailmod);
      if(detail<1)detail=1;
      mainimage(buckets,numbuckets,detail);
//      previewimage(buckets,numbuckets,detail);
   }
   else if(imagetype==2){
      detail=(int)(numbuckets*previewdetailmod);
      if(detail<1)detail=1;
      previewimage(buckets,numbuckets,detail);
   }
   return true;
}

//This determines what part of the image is displayed with orthographic projection. It sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area from the dimensions of the window. *ratio*zoomlevel is there to convert screen dimensions to image dimensions. gluLookAt is then used so that the viewpoint is that of seeing the centre from a position to the left of the profile, when looking from the start to the end of it.
void Profile::resetview(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-(get_width()/2)*ratio/zoomlevel,
          +(get_width()/2)*ratio/zoomlevel,
          -(get_height()/2)*ratio/zoomlevel,
          +(get_height()/2)*ratio/zoomlevel,
          -5*width,
          +5*width);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  double breadth = endx - startx;
  double height = endy - starty;
  double length = sqrt(breadth*breadth+height*height);//Right triangle.
  viewerx = centrex + width * height / length;//To the right when looking from start to end.
  viewery = centrey - width * breadth / length;//...
  viewerz = centrez;
  gluLookAt(viewerx,viewery,viewerz,
            centrex,centrey,centrez,
            0,0,1);
}

//Draw on expose. 1 indicates that the non-preview image is drawn.
bool Profile::on_expose_event(GdkEventExpose* event){ return drawviewable(1); }

//On a left click, this prepares for panning by storing the initial position of the cursor.
bool Profile::on_pan_start(GdkEventButton* event){
   if(event->button==1){
      panstartx = event->x;
      panstarty = event->y;
   }
   return true;
}

//As the cursor moves while the left button is depressed, the image is dragged along as a preview (with fewer points) to reduce lag. The centre point is modified by the negative of the distance (in image units, hence the ratio/zoomlevel mention) the cursor has moved to make a dragging effect and then the current position of the cursor is taken to be the starting position for the next drag (if there is one). The view is then refreshed and then the image is drawn (as a preview).
bool Profile::on_pan(GdkEventMotion* event){
//Y is reversed because gtk has origin at top left and opengl has it at bottom left.
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = (event->x-panstartx)*ratio/zoomlevel;
   centrex -= hypotenuse * breadth / length;
   centrey -= hypotenuse * height / length;
   centrez += (event->y-panstarty)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   panstartx=event->x;
   panstarty=event->y;
   resetview();
   return drawviewable(2);
}

//At the end of the pan draw the full image.
bool Profile::on_pan_end(GdkEventButton* event){
   if(event->button==1)return drawviewable(1);
   else return false;
}

//Find the starting coordinates of the ruler and set the label values to zero.
bool Profile::on_ruler_start(GdkEventButton* event){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;
   rulerstartx = rulerendx = viewerx + hypotenuse * breadth / length;
   rulerstarty = rulerendy = viewery + hypotenuse * height / length;
   rulerstartz = rulerendz = viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   rulerlabel->set_text("Distance: 0\nX: 0\nY: 0\nHoriz: 0\nZ: 0");
   makerulerbox();
   return drawviewable(1);
}

//Find the current cursor coordinates in image terms (as opposed to window/screen terms) and then update the label with the distances. Then draw the ruler.
bool Profile::on_ruler(GdkEventMotion* event){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;
   rulerendx = viewerx + hypotenuse * breadth / length;
   rulerendy = viewery + hypotenuse * height / length;
   rulerendz = viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   double d,xd,yd,hd,zd;
   xd = abs(rulerendx-rulerstartx);
   yd = abs(rulerendy-rulerstarty);
   zd = abs(rulerendz-rulerstartz);
   hd = sqrt(xd*xd+yd*yd);
   d = sqrt(hd*hd+zd*zd);
   ostringstream dist,xdist,ydist,horizdist,zdist;
   dist << d;
   xdist << xd;
   ydist << yd;
   horizdist << hd;
   zdist << zd;
   string rulerstring = "Distance: " + dist.str() +"\nX: " + xdist.str() + "\nY: " + ydist.str() + "\nHoriz: " + horizdist.str() + "\nZ: " + zdist.str();
   rulerlabel->set_text(rulerstring);
   makerulerbox();
   return drawviewable(1);
}

//Draw again. This is for if/when the on_ruler() method calls drawviewable(2) rather than drawviewable(1).
bool Profile::on_ruler_end(GdkEventButton* event){return drawviewable(1);}

//Make the ruler as a thick line.
void Profile::makerulerbox(){
   glNewList(4,GL_COMPILE);
   glColor3f(1.0,1.0,1.0);
   glLineWidth(3);
   glBegin(GL_LINES);
      glVertex3d(rulerstartx,rulerstarty,rulerstartz);
      glVertex3d(rulerendx,rulerendy,rulerendz);
   glEnd();
   glLineWidth(1);
   glEndList();
}

//First, half the distance between the centre of the window and the window position of the event is converted to image coordinates and added to the image centre. This is analogous to moving the centre to where the event occured. Then, depending on the direction of the scroll, the zoomlevel is increased or decreased. Then the centre is moved to where the centre of the window will now lie. The image is then drawn.
bool Profile::on_zoom(GdkEventScroll* event){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse;
   hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;
   centrex += hypotenuse * breadth / length;
   centrey += hypotenuse * height / length;
   centrez -= (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   if(zoomlevel>=1){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=pow(zoomlevel,zoompower)/2;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=pow(zoomlevel,zoompower)/2;
   }
   else if(zoomlevel>=0.2){
      if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
      else if(event->direction==GDK_SCROLL_DOWN)zoomlevel-=0.1;
   }
   else if(event->direction==GDK_SCROLL_UP)zoomlevel+=0.1;
   if(zoomlevel<0.2)zoomlevel=0.2;
   hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;
   centrex -= hypotenuse * breadth / length;
   centrey -= hypotenuse * height / length;
   centrez += (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   resetview();
   return drawviewable(1);
}

//When the window is resized, the viewport is resized accordingly and so are teh viewing properties.
bool Profile::on_configure_event(GdkEventConfigure* event){
  glViewport(0, 0, get_width(), get_height());
  resetview();
  return true;
}

bool Profile::linecomp(point* a,point* b){
   double xa = a->x;
   double xb = b->x;
   double ya = a->y;
   double yb = b->y;
   double alongprofa,alongprofb;
   if(startx==endx){//If the profile is parallel to the y axis:
      double mult=-1;
      if(starty<endy)mult=1;
      alongprofa = mult * (ya - starty);
      alongprofb = mult * (yb - starty);
   }
   else if(starty==endy){//If the profile is parallel to the x axis:
      double mult=-1;
      if(startx<endx)mult=1;
      alongprofa = mult * (xa - startx);
      alongprofb = mult * (xb - startx);
   }
   else{//If the profile is skewed:
      double breadth = endx - startx;
      double height = endy - starty;
      double multx=-1;
      if(startx<endx)multx=1;
      double multy=-1;
      if(starty<endy)multy=1;
      double lengradbox = multx * multy * height / breadth;//Gradients of the profile and point-to-profile lines
      double widgradbox = -1.0 / lengradbox;//...
      double widgradboxa = multy * (ya - starty) - (multx * (xa - startx) * widgradbox);//Constant values (y intercept) of the formulae for lines from each point to the profile line.
      double widgradboxb = multy * (yb - starty) - (multx * (xb - startx) * widgradbox);//...
      //Testing points:
      double interxa,interxb,interya,interyb;
      interxa = widgradboxa / (widgradbox - lengradbox);
      interya = interxa * lengradbox;
      interxb = widgradboxb / (widgradbox - lengradbox);
      interyb = interxb * lengradbox;
      alongprofa = sqrt(interxa*interxa+interya*interya);
      alongprofb = sqrt(interxb*interxb+interyb*interyb);
   }
   return alongprofa > alongprofb;
}

/*This method draws the main image. First, the gl_window is acquired for drawing. It is then cleared, otherwise the method would just draw over the previous image and, since this image will probably have gaps in it, the old image would be somewhat visible. Then:
 *
 *   for every bucket:
 *      for every point:
 *         determine colour and brightness of point
 *         place point
 *      end for
 *      draw all points in bucket
 *   end for
 *
 *Then the profiling box is drawn if it exists.
 *
 *
 * */
bool Profile::mainimage(pointbucket** buckets,int numbuckets,int detail){
   vector<int> flightlines;
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   int line=0,intensity=0,classification,rnumber;
   double red,green,blue;
   double x=0,y=0,z=0;
   float* vertices = new float[3*bucketlimit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*bucketlimit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   for(int i=0;i<numbuckets;i++){//For every bucket...
      int count=0;//This is needed for putting values in the right indices for the above arrays. j does not suffice because of the detail variable.
      for(int j=0;j<buckets[i]->numberofpoints;j+=detail){//... and for every point, determine point colour and position:
         if(correctpointsbuckets[i][j]){
            if(drawpoints){
               red = 0.0; green = 1.0; blue = 0.0;//Default colour.
               x = buckets[i]->points[j].x;
               y = buckets[i]->points[j].y;
               z = buckets[i]->points[j].z;
               intensity = buckets[i]->points[j].intensity;
               if(heightcolour){//Colour by elevation.
                  red = colourheightarray[3*(int)(10*(z-rminz))];
                  green = colourheightarray[3*(int)(10*(z-rminz)) + 1];
                  blue = colourheightarray[3*(int)(10*(z-rminz)) + 2];
               }
               else if(intensitycolour){//Colour by intensity.
                  red = colourintensityarray[3*(int)(intensity-rminintensity)];
                  green = colourintensityarray[3*(int)(intensity-rminintensity) + 1];
                  blue = colourintensityarray[3*(int)(intensity-rminintensity) + 2];
               }
               else if(linecolour){//Colour by flightline. Repeat 6 distinct colours.
                   line = buckets[i]->points[j].flightline;
                   int index = line % 6;
                   switch(index){
                      case 0:red=0;green=1;blue=0;break;//Green
                      case 1:red=0;green=0;blue=1;break;//Blue
                      case 2:red=1;green=0;blue=0;break;//Red
                      case 3:red=0;green=1;blue=1;break;//Cyan
                      case 4:red=1;green=1;blue=0;break;//Yellow
                      case 5:red=1;green=0;blue=1;break;//Purple
                      default:red=green=blue=1;break;//White in the event of strangeness.
                   }
               }
               else if(classcolour){//Colour by classification.
                   classification = buckets[i]->points[j].classification;
                   int index = classification;
                   switch(index){
                      case 0:case 1:red=1;green=0;blue=0;break;//Red for non-classified.
                      case 2:red=0.6;green=0.3;blue=0;break;//Brown for ground.
                      case 3:red=0;green=0.3;blue=0;break;//Dark green for low vegetation.
                      case 4:red=0;green=0.6;blue=0;break;//Medium green for medium vegetation.
                      case 5:red=0;green=1;blue=0;break;//Bright green for high vegetation.
                      case 6:red=0;green=1;blue=0;break;//Cyan for buildings.
                      case 7:red=1;green=0;blue=1;break;//Purple for low point (noise).
                      case 8:red=0.5;green=0.5;blue=0.5;break;//Grey for model key-point (mass point).

                      case 9:red=0;green=0;blue=1;break;//Blue for water.
                      case 12:red=1;green=1;blue=1;break;//White for overlap points.
                      default:red=1;green=1;blue=0;cout << "Undefined point." << endl;break;//Yellow for undefined.
                   }
               }
               else if(returncolour){//Colour by flightline. Repeat 6 distinct colours.
                   rnumber = buckets[i]->points[j].rnumber;
                   int index = rnumber;
                   switch(index){
                      case 1:red=0;green=0;blue=1;break;//Blue
                      case 2:red=0;green=1;blue=1;break;//Cyan
                      case 3:red=0;green=1;blue=0;break;//Green
                      case 4:red=1;green=0;blue=0;break;//Red
                      case 5:red=1;green=0;blue=1;break;//Purple
                      default:red=green=blue=1;break;//White in the event of strangeness.
                   }
               }
               if(heightbrightness){//Shade by height.
                  red *= brightnessheightarray[(int)(z-rminz)];
                  green *= brightnessheightarray[(int)(z-rminz)];
                  blue *= brightnessheightarray[(int)(z-rminz)];
               }
               else if(intensitybrightness){//Shade by intensity.
                  red *= brightnessintensityarray[(int)(intensity-rminintensity)];
                  green *= brightnessintensityarray[(int)(intensity-rminintensity)];
                  blue *= brightnessintensityarray[(int)(intensity-rminintensity)];
               }
               vertices[3*count]=x;
               vertices[3*count+1]=y;
               vertices[3*count+2]=z;
               colours[3*count]=red;
               colours[3*count+1]=green;
               colours[3*count+2]=blue;
               count++;
            }
            line = buckets[i]->points[j].flightline;
            if(drawmovingaverage)if(find(flightlines.begin(),flightlines.end(),line)==flightlines.end())flightlines.push_back(line);
         }
      }
      glDrawArrays(GL_POINTS,0,count);
//      if (glwindow->is_double_buffered())glwindow->swap_buffers();//Draw to screen every bucket to show user stuff is happening.
//      else glFlush();
   }
   if(rulering)glCallList(4);//Draw the ruler if ruler mode is on.
   if(drawmovingaverage){
      for(int i=0;i<(int)flightlines.size();i++){
         int count = 0;
         vector<point*> flightlinepoints;
         int index = flightlines.at(i) % 6;
         switch(index){
            case 0:red=0;green=1;blue=0;break;//Green
            case 1:red=0;green=0;blue=1;break;//Blue
            case 2:red=1;green=0;blue=0;break;//Red
            case 3:red=0;green=1;blue=1;break;//Cyan
            case 4:red=1;green=1;blue=0;break;//Yellow
            case 5:red=1;green=0;blue=1;break;//Purple
            default:red=green=blue=1;break;//White in the event of strangeness.
         }
         for(int j=0;j<numbuckets;j++){
            for(int k=0;k<buckets[j]->numberofpoints;k++){
               if(correctpointsbuckets[j][k])if(buckets[j]->points[k].flightline == flightlines.at(i))flightlinepoints.push_back(&(buckets[j]->points[k]));
            }
         }
         sort(flightlinepoints.begin(),flightlinepoints.end(),boost::bind(&Profile::linecomp,this,_1,_2));
//         glColor3d(red,green,blue);
//         glBegin(GL_LINE_STRIP);
         for(int j=0;j<(int)flightlinepoints.size();j++){
            double z=0,zcount=0;
            for(int k=-mavrgrange;k<=mavrgrange;k++)if((j+k>=0&&j+k<(int)flightlinepoints.size())){
               z+=flightlinepoints.at(j+k)->z;
               zcount++;
            }
            z /= zcount;
//            glVertex3d(flightlinepoints.at(j)->x,flightlinepoints.at(j)->y,z);
            double x = flightlinepoints.at(j)->x;
            double y = flightlinepoints.at(j)->y;
            vertices[3*count]=x;
            vertices[3*count+1]=y;
            vertices[3*count+2]=z;
            colours[3*count]=red;
            colours[3*count+1]=green;
            colours[3*count+2]=blue;
            count++;
         }
//         glEnd();
         glDrawArrays(GL_LINE_STRIP,0,count);
      }
   }
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}

//NOTE: This method is almost identical to the mainimage method. Only two lines are different, and could easily be replace with an if statement. The methods are kept separate as this one might change in the future to make it faster and/or more detailed.
/*This method draws the preview used in panning etc.. Basically, if something must be drawn quickly, this method is used. First, the gl_window is acquired for drawing. It is then cleared, otherwise the method would just draw over the previous image and, since this image will probably have gaps in it, the old image would be somewhat visible. Then:
 *
 *   for every bucket:
 *      for every point:
 *         determine colour and brightness of point
 *         place point
 *      end for
 *   end for
 *   draw all points
 *
 *Then the profiling box is drawn if it exists.
 *
 *
 * */
bool Profile::previewimage(pointbucket** buckets,int numbuckets,int detail){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   double red,green,blue;
   double x=0,y=0,z=0;
   int line=0,intensity=0,classification,rnumber;
   float* vertices = new float[3*bucketlimit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*bucketlimit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   for(int i=0;i<numbuckets;i++){//For every bucket...
      int count=0;//This is needed for putting values in the right indices for the above arrays. j does not suffice because of the detail variable.
      for(int j=0;j<buckets[i]->numberofpoints;j+=detail){//... and for every point, determine point colour and position:
         if(correctpointsbuckets[i][j]){
            red = 0.0; green = 1.0; blue = 0.0;//Default colour.
            x = buckets[i]->points[j].x;
            y = buckets[i]->points[j].y;
            z = buckets[i]->points[j].z;
            intensity = buckets[i]->points[j].intensity;
            if(heightcolour){//Colour by elevation.
               red = colourheightarray[3*(int)(10*(z-rminz))];
               green = colourheightarray[3*(int)(10*(z-rminz)) + 1];
               blue = colourheightarray[3*(int)(10*(z-rminz)) + 2];
            }
            else if(intensitycolour){//Colour by intensity.
               red = colourintensityarray[3*(int)(intensity-rminintensity)];
               green = colourintensityarray[3*(int)(intensity-rminintensity) + 1];
               blue = colourintensityarray[3*(int)(intensity-rminintensity) + 2];
            }
            else if(linecolour){//Colour by flightline. Repeat 6 distinct colours.
                line = buckets[i]->points[j].flightline;
                int index = line % 6;
                switch(index){
                   case 0:red=0;green=1;blue=0;break;//Green
                   case 1:red=0;green=0;blue=1;break;//Blue
                   case 2:red=1;green=0;blue=0;break;//Red
                   case 3:red=0;green=1;blue=1;break;//Cyan
                   case 4:red=1;green=1;blue=0;break;//Yellow
                   case 5:red=1;green=0;blue=1;break;//Purple
                   default:red=green=blue=1;break;//White in the event of strangeness.
                }
            }
            else if(classcolour){//Colour by classification.
                classification = buckets[i]->points[j].classification;
                int index = classification;
                switch(index){
                   case 0:case 1:red=1;green=0;blue=0;break;//Red for non-classified.
                   case 2:red=0.6;green=0.3;blue=0;break;//Brown for ground.
                   case 3:red=0;green=0.3;blue=0;break;//Dark green for low vegetation.
                   case 4:red=0;green=0.6;blue=0;break;//Medium green for medium vegetation.
                   case 5:red=0;green=1;blue=0;break;//Bright green for high vegetation.
                   case 6:red=0;green=1;blue=0;break;//Cyan for buildings.
                   case 7:red=1;green=0;blue=1;break;//Purple for low point (noise).
                   case 8:red=0.5;green=0.5;blue=0.5;break;//Grey for model key-point (mass point).

                   case 9:red=0;green=0;blue=1;break;//Blue for water.
                   case 12:red=1;green=1;blue=1;break;//White for overlap points.
                   default:red=1;green=1;blue=0;cout << "Undefined point." << endl;break;//Yellow for undefined.
                }
            }
            else if(returncolour){//Colour by flightline. Repeat 6 distinct colours.
                rnumber = buckets[i]->points[j].rnumber;
                int index = rnumber;
                switch(index){
                   case 1:red=0;green=0;blue=1;break;//Blue
                   case 2:red=0;green=1;blue=1;break;//Cyan
                   case 3:red=0;green=1;blue=0;break;//Green
                   case 4:red=1;green=0;blue=0;break;//Red
                   case 5:red=1;green=0;blue=1;break;//Purple
                   default:red=green=blue=1;break;//White in the event of strangeness.
                }
            }
            if(heightbrightness){//Shade by height.
               red *= brightnessheightarray[(int)(z-rminz)];
               green *= brightnessheightarray[(int)(z-rminz)];
               blue *= brightnessheightarray[(int)(z-rminz)];
            }
            else if(intensitybrightness){//Shade by intensity.
               red *= brightnessintensityarray[(int)(intensity-rminintensity)];
               green *= brightnessintensityarray[(int)(intensity-rminintensity)];
               blue *= brightnessintensityarray[(int)(intensity-rminintensity)];
            }
            vertices[3*count]=x;
            vertices[3*count+1]=y;
            vertices[3*count+2]=z;
            colours[3*count]=red;
            colours[3*count+1]=green;
            colours[3*count+2]=blue;
            count++;
         }
      }
      glDrawArrays(GL_POINTS,0,count);
   }
   if(rulering)glCallList(4);//Draw the ruler if ruler mode is on.
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}

//Prepares the arrays for looking up the colours and shades of the points.
void Profile::coloursandshades(double maxz,double minz,int maxintensity,int minintensity){
   delete[] colourheightarray;
   delete[] colourintensityarray;
   delete[] brightnessheightarray;
   delete[] brightnessintensityarray;
   double red=0.0,green=0.0,blue=0.0;
   double z=0,intensity=0;
   colourheightarray = new double[30*(int)(rmaxz-rminz+4)];
   for(int i=0;i<(int)(10*(rmaxz-rminz)+3);i++){//Fill height colour array:
      z = 0.1*(double)i + rminz;
      colour_by(z,maxz,minz,red,green,blue);
      colourheightarray[3*i]=red;
      colourheightarray[3*i+1]=green;
      colourheightarray[3*i+2]=blue;
   }
   colourintensityarray = new double[3*(int)(rmaxintensity-rminintensity+4)];
   for(int i=0;i<rmaxintensity-rminintensity+3;i++){//Fill intensity colour array:
      intensity = i + rminintensity;
      colour_by(intensity,maxintensity,minintensity,red,green,blue);
      colourintensityarray[3*i]=red;
      colourintensityarray[3*i+1]=green;
      colourintensityarray[3*i+2]=blue;
   }
   brightnessheightarray = new double[(int)(rmaxz-rminz+4)];
   for(int i=0;i<(int)(rmaxz-rminz)+3;i++){//Fill height brightness array:
      z = i + (int)rminz;
      brightnessheightarray[i] = brightness_by(z,maxz,minz,zoffset,zfloor);
   }
   brightnessintensityarray = new double[(int)(rmaxintensity-rminintensity+4)];
   for(int i=0;i<rmaxintensity-rminintensity+3;i++){//Fill intensity brightness array:
      intensity = i + rminintensity;
      brightnessintensityarray[i] = brightness_by(intensity,maxintensity,minintensity,intensityoffset,intensityfloor);
   }
}

//This method prepares the image for drawing and sets up OpenGl. It gets the data from the quadtree in order to find the maximum and minimum height and intensity values and calls the coloursandshades() method to prepare the colouring of the points. It also sets ups anti-aliasing, clearing and the initial view.
void Profile::prepare_image(){
   //Initial state:
   lidarboundary = lidardata->getboundary();
   double xdif = lidarboundary->maxX-lidarboundary->minX;
   double ydif = lidarboundary->maxY-lidarboundary->minY;
      //Initial centre:
      viewerx = centrex = lidarboundary->minX+xdif/2;
      viewery = centrey = lidarboundary->minY+ydif/2;
      viewerz = centrez = 0;
      //Scaling to screen dimensions:
      double swidth = get_screen()->get_width();
      double sheight = get_screen()->get_height();
      double xratio = xdif/swidth;
      double yratio = ydif/sheight;
      yratio*=1.25;
      ratio = 0;
      if(xratio>yratio)ratio = xratio;
      else ratio = yratio;
   vector<pointbucket*> *ppointvector;
   try{
      ppointvector = lidardata->subset(lidarboundary->minX,lidarboundary->minY,lidarboundary->maxX,lidarboundary->maxY);//Get ALL data.
   }catch(const char* e){
      cout << e << endl;
      cout << "No points returned." << endl;
      return;
   }
   int numpbuckets = ppointvector->size();
   pointbucket** pbuckets = new pointbucket*[numpbuckets];
   for(int i=0;i<numpbuckets;i++){
      pbuckets[i]=ppointvector->at(i);
   }
   double maxz = pbuckets[0]->maxz,minz = pbuckets[0]->minz;
   int maxintensity = pbuckets[0]->maxintensity,minintensity = pbuckets[0]->minintensity;
   for(int i=0;i<numpbuckets;i++){//Find the maximum and minimum values from the pbuckets:
      if(maxz<pbuckets[i]->maxz)maxz = pbuckets[i]->maxz;
      if(minz>pbuckets[i]->minz)minz = pbuckets[i]->minz;
      if(maxintensity<pbuckets[i]->maxintensity)maxintensity = pbuckets[i]->maxintensity;
      if(minintensity>pbuckets[i]->minintensity)minintensity = pbuckets[i]->minintensity;
   }
   rmaxz = maxz; rminz = minz;
   centrez = rminz + (rmaxz + rminz) / 2;
   rmaxintensity = maxintensity; rminintensity = minintensity;
   if(maxz<=minz)maxz=minz+1;
   else{//Find the 0.01 and 99.99 percentiles of the height and intensity from the pbuckets. Not perfect (it could miss a hill in a bucket) but it does the job reasonably well:
      double* zdata = new double[numpbuckets];
      for(int i=0;i<numpbuckets;i++)zdata[i]=pbuckets[i]->maxz;
      double lowperc = percentilevalue(zdata,numpbuckets,0.01,minz,maxz);
      for(int i=0;i<numpbuckets;i++)zdata[i]=pbuckets[i]->minz;
      double highperc = percentilevalue(zdata,numpbuckets,99.99,minz,maxz);
      maxz=highperc;
      minz=lowperc;
      delete[] zdata;
   }
   coloursandshades(maxz,minz,maxintensity,minintensity);//Prepare colour and shading arrays.
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return;
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearDepth(1.0);
//   glEnable(GL_POINT_SMOOTH);//Antialiasing stuff, for use later, possibly. NOTE: Currently causes bucket shaped graphical artefacts. Obviously, anti-aliasing is happening when the data is passed to opengl, not when flush happens.
//   glEnable(GL_LINE_SMOOTH);//...
//   glEnable(GL_BLEND);//...
//   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);//...
//   glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);//...
//   glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);//...
   glPointSize(pointsize);//This is to be user-changeable later as, depending on the screen resolution, single pixels can be hard to see properly.
   glEnable(GL_DEPTH_TEST);//Very important to include this! This allows us to see the things on the top above the things on the bottom!
   glViewport(0, 0, get_width(), get_height());
   resetview();
   delete[] pbuckets;
   delete ppointvector;
   glwindow->gl_end();
}

//Prepare the image when the widget is first realised.
void Profile::on_realize(){
  Gtk::GL::DrawingArea::on_realize();
  glcontext = Gdk::GL::Context::create(this->get_gl_window(),true,Gdk::GL::RGBA_TYPE);
  cout << this->get_gl_window()->make_current(glcontext) << endl;
  prepare_image();
}

//Given maximum and minimum values, find out the colour a certain value should be mapped to.
void Profile::colour_by(double value,double maxvalue,double minvalue,double& col1,double& col2,double& col3){//Following comments assume col1=red, col2=green and col3=blue.
  double range = maxvalue-minvalue;
  if(value<=minvalue+range/6){//Green to Yellow:
     col1 = 6*(value-minvalue)/range;
     col2 = 1.0;
     col3 = 0.0;
  }
  else if(value<=minvalue+range/3){//Yellow to Red:
     col1 = 1.0;
     col2 = 2.0 - 6*(value-minvalue)/range;
     col3 = 0.0;
  }
  else if(value<=minvalue+range/2){//Red to Purple:
     col1 = 1.0;
     col2 = 0.0;
     col3 = 6*(value-minvalue)/range - 2.0;
  }
  else if(value<=minvalue+range*2/3){//Purple to Blue:
     col1 = 4.0 - 6*(value-minvalue)/range;
     col2 = 0.0;
     col3 = 1.0;
  }
  else if(value<=minvalue+range*5/6){//Blue to Cyan:
     col1 = 0.0;
     col2 = 6*(value-minvalue)/range - 4.0;
     col3 = 1.0;
  }
  else{//Cyan to White:
     col1 = 6*(value-minvalue)/range - 5.0;
     if(col1>1.0)col1=1.0;
     col2 = 1.0;
     col3 = 1.0;
  }
}

//Given maximum and minimum values, find out the brightness a certain value should be mapped to.
double Profile::brightness_by(double value,double maxvalue,double minvalue,double offsetvalue,double floorvalue){
  double multiplier = floorvalue + offsetvalue + (1.0 - offsetvalue)*(value-minvalue)/(maxvalue-minvalue);
  return multiplier;
}
