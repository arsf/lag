/*
 * File: Profile.cpp
 * Author: Haraldur Tristan Gunnarsson
 * Written: December 2009 - January 2010
 *
 * */
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

Profile::Profile(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel)  : Display(config,lidardata,bucketlimit){
   flightlinepoints = new vector<point*>[1];
   viewerz = 0;
   zoompower = 0.7;
   imageexists=false;
   maindetailmod = 0;
   previewdetailmod = 0.3;
   drawpoints = true;
   drawmovingaverage = false;
   mavrgrange = 5;
   startx = 0;
   starty = 0;
   buckets = new pointbucket*[2];
   correctpointsbuckets = new bool*[2];
   correctpointsbuckets[0] = new bool[2];
   correctpointsbuckets[1] = new bool[2];
   numbuckets = 2;
   linezsize=1;
   linez = new double*[linezsize];
   linez[0] = new double[1];
   //Rulering:
   rulering=false;
   rulerwidth=2;
   this->rulerlabel = rulerlabel;
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

Profile::~Profile(){}

bool Profile::returntostart(){
   centrex = (startx + endx)/2;
   centrey = (starty + endy)/2;
   centrez=0;
   for(int i=0;i<(int)flightlinestot.size();i++){
      centrez+=flightlinepoints[i][0]->z;
      centrez+=flightlinepoints[i][flightlinepoints[i].size()-1]->z;
   }
   centrez/=(2*flightlinestot.size());
   zoomlevel=1;
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   viewerx = width * height / length;//To the right when looking from start to end.
   viewery = -width * breadth / length;//...
   ratio = length/get_width();
   ratio*=1.1;
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
   for(int i=0;i<numbuckets;i++)delete[] correctpointsbuckets[i];
   delete[] correctpointsbuckets;
//   delete[] buckets;
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
   buckets = &(*pointvector)[0];
//   buckets = new pointbucket*[numbuckets];
   flightlinestot.clear();
   correctpointsbuckets = new bool*[numbuckets];//Determines whether points are in the profile and, therefore, whether they are drawn.
   for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
//      buckets[i]=pointvector->at(i);
      correctpointsbuckets[i] = vetpoints(buckets[i]->numberofpoints,buckets[i]->points,startx,starty,endx,endy,width);
      for(int j=0;j<buckets[i]->numberofpoints;j++){
         if(correctpointsbuckets[i][j]){
            if(find(flightlinestot.begin(),flightlinestot.end(),buckets[i]->points[j].flightline)==flightlinestot.end()){
               flightlinestot.push_back(buckets[i]->points[j].flightline);
            }
         }
      }
   }
   delete[] flightlinepoints;
   flightlinepoints = new vector<point*>[flightlinestot.size()];
   for(int i=0;i<(int)flightlinestot.size();i++){
      for(int j=0;j<numbuckets;j++){//Get all points that should be accounted for:
         for(int k=0;k<buckets[j]->numberofpoints;k++){//Possibly: do k+=detail instead, and copy to preview. Might not be "correct" though.
            if(correctpointsbuckets[j][k]){
               if(buckets[j]->points[k].flightline == flightlinestot.at(i))flightlinepoints[i].push_back(&(buckets[j]->points[k]));
            }
         }
      }
      minplanx = startx;
      minplany = starty;
      sort(flightlinepoints[i].begin(),flightlinepoints[i].end(),boost::bind(&Profile::linecomp,this,_1,_2));//Sort so that lines are intelligible and right.
   }
   make_moving_average();
   glViewport(0, 0, get_width(), get_height());
   get_gl_window()->make_current(get_gl_context());
   delete pointvector;
   if(is_realized())return returntostart();
   else return false;
}

//This determines what part of the image is displayed with orthographic projection. It sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area from the dimensions of the window. *ratio*zoomlevel is there to convert screen dimensions to image dimensions. gluLookAt is then used so that the viewpoint is that of seeing the centre from a position to the left of the profile, when looking from the start to the end of it.
void Profile::resetview(){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   leftboundx = -((get_width()/2)*ratio/zoomlevel) * breadth / length;
   rightboundx = ((get_width()/2)*ratio/zoomlevel) * breadth / length;
   leftboundy = -((get_width()/2)*ratio/zoomlevel) * height / length;
   rightboundy = ((get_width()/2)*ratio/zoomlevel) * height / length;
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
   gluLookAt(viewerx,viewery,viewerz,
             0,0,0,
             0,0,1);
//   GLint viewport[4];
//   glGetIntegerv(GL_VIEWPORT,viewport);
//   GLdouble modelview[16];
//   glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
//   GLdouble projection[16];
//   glGetDoublev(GL_PROJECTION_MATRIX,projection);
//   GLdouble z=0,zs=0,y=0;
//   gluUnProject(0,y,z,modelview,projection,viewport,&leftboundx,&leftboundy,&zs);
//   gluUnProject(get_width(),y,z,modelview,projection,viewport,&rightboundx,&rightboundy,&zs);
}

int Profile::get_closest_element_position(point* value,vector<point*>::iterator first,vector<point*>::iterator last){
   vector<point*>::iterator originalFirst = first;
   vector<point*>::iterator middle;
   while(true){//INFINITE LOOP interrupted by returns.
      middle = first + distance(first,last)/2;
      if(linecomp(*middle,value))first = middle;
      else if(linecomp(value,*middle))last = middle;
      else return distance(originalFirst,middle);
      if(distance(first,last)<2 && distance(first,middle)<1)return distance(originalFirst,middle);
   }
}

//Depending on the imagetype requested, this sets the detail level and then calls one of the image methods, which actually draws the data to the screen.
bool Profile::drawviewable(int imagetype){
//   double breadth = endx - startx;
//   double height = endy - starty;
//   double length = sqrt(breadth*breadth+height*height);//Right triangle.
//   double hypotenuse,length2;
//   double starcenx = centrex - startx;
//   double starceny = centrey - starty;
//   double endcenx = endx - centrex;
//   double endceny = endy - centrey;
//   if(starcenx*breadth<0)starcenx=0;
//   if(starceny*breadth<0)starceny=0;
//   if(endcenx*breadth<0)endcenx=0;
//   if(endceny*breadth<0)endceny=0;
//   cout << "See1:" << endl;
//   cout << starcenx << endl;
//   cout << starceny << endl;
//   cout << endcenx << endl;
//   cout << endceny << endl;
//   hypotenuse = (get_width()/2)*ratio/zoomlevel;
//   length2 = sqrt((starcenx)*(starcenx) + (starceny)*(starceny));
//   if(hypotenuse>=length2)hypotenuse = length2;
//   double vstartx = centrex - hypotenuse * breadth / length;
//   double vstarty = centrey - hypotenuse * height / length;
//   hypotenuse = (get_width()/2)*ratio/zoomlevel;
//   length2 = sqrt((endcenx)*(endcenx) + (endceny)*(endceny));
//   if(hypotenuse>=length2)hypotenuse = length2;
//   double vendx = centrex + hypotenuse * breadth / length;
//   double vendy = centrey + hypotenuse * height / length;
//   for(int i=0;i<numbuckets;i++)delete[] correctpointsbuckets[i];
//   delete[] correctpointsbuckets;
//   delete[] buckets;
//   vector<pointbucket*> *pointvector;
//   try{
//      pointvector = lidardata->advsubset(vstartx,vstarty,vendx,vendy,width);//Get data.
//      imageexists=true;
//   }catch(const char* e){
//      cout << e << endl;
//      cout << "No points returned." << endl;
//      imageexists=false;
//      return false;
//   }
//   numbuckets = pointvector->size();
//   buckets = new pointbucket*[numbuckets];
//   correctpointsbuckets = new bool*[numbuckets];//Determines whether points are in the profile and, therefore, whether they are drawn.
//   for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
//      buckets[i]=pointvector->at(i);
//      correctpointsbuckets[i] = vetpoints(buckets[i]->numberofpoints,buckets[i]->points,vstartx,vstarty,vendx,vendy,width);
//   }
//   delete pointvector;
   if(!imageexists){//If there is an attempt to draw with no data, the program will probably crash.
      Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
      if (!glwindow->gl_begin(get_gl_context()))return false;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
      if (glwindow->is_double_buffered())glwindow->swap_buffers();
      else glFlush();
      return false;
   }
  glViewport(0, 0, get_width(), get_height());
  get_gl_window()->make_current(get_gl_context());
  resetview();
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
   rulerstartx = rulerendx = centrex + viewerx + hypotenuse * breadth / length;
   rulerstarty = rulerendy = centrey + viewery + hypotenuse * height / length;
   rulerstartz = rulerendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
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
   rulerendx = centrex + viewerx + hypotenuse * breadth / length;
   rulerendy = centrey + viewery + hypotenuse * height / length;
   rulerendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
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
//   makerulerbox();
   return drawviewable(1);
}
//Draw again. This is for if/when the on_ruler() method calls drawviewable(2) rather than drawviewable(1).
bool Profile::on_ruler_end(GdkEventButton* event){return drawviewable(1);}
//Make the ruler as a thick line.
void Profile::makerulerbox(){
   glColor3f(1.0,1.0,1.0);
   glLineWidth(3);
   glBegin(GL_LINES);
      glVertex3d(rulerstartx-centrex,rulerstarty-centrey,rulerstartz-centrez);
      glVertex3d(rulerendx-centrex,rulerendy-centrey,rulerendz-centrez);
   glEnd();
   glLineWidth(1);
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

//This method is for sort(). It projects the points onto a plane defined by the z axis and the line perpendicular to the viewing direction.
bool Profile::linecomp(point *a,point *b){
   double xa = a->x;
   double xb = b->x;
   double ya = a->y;
   double yb = b->y;
   double alongprofa,alongprofb;
   if(startx==endx){//If the profile is parallel to the y axis:
      double mult=-1;//Used so that points are projecting onto the right side (NOT face) of the plane.
      if(starty<endy)mult=1;
      alongprofa = mult * (ya - minplany);
      alongprofb = mult * (yb - minplany);
   }
   else if(starty==endy){//If the profile is parallel to the x axis:
      double mult=-1;//Used so that points are projecting onto the right side (NOT face) of the plane.
      if(startx<endx)mult=1;
      alongprofa = mult * (xa - minplanx);
      alongprofb = mult * (xb - minplanx);
   }
   else{//If the profile is skewed:
      double breadth = endx - startx;
      double height = endy - starty;
      double multx=-1;//Used so that points are projecting onto the right side (NOT face) of the plane.
      if(startx<endx)multx=1;
      double multy=-1;//Used so that points are projecting onto the right side (NOT face) of the plane.
      if(starty<endy)multy=1;
      double lengradbox = multx * multy * height / breadth;//Gradients of the profile and point-to-profile lines
      double widgradbox = -1.0 / lengradbox;//...
      double widgradboxa = multy * (ya - minplany) - (multx * (xa - minplanx) * widgradbox);//Constant values (y intercept) of the formulae for lines from each point to the profile line.
      double widgradboxb = multy * (yb - minplany) - (multx * (xb - minplanx) * widgradbox);//...
      //Testing points:
      double interxa,interxb,interya,interyb;
      interxa = widgradboxa / (widgradbox - lengradbox);//The x (intercept with plane) value of the line from the point to the plane.
      interya = interxa * lengradbox;//The y (intercept with plane) value of the line from the point to the plane.
      interxb = widgradboxb / (widgradbox - lengradbox);//The x (intercept with plane) value of the line from the point to the plane.
      interyb = interxb * lengradbox;//The y (intercept with plane) value of the line from the point to the plane.
      alongprofa = sqrt(interxa*interxa+interya*interya);//Use the values of x and y as well as pythagoras to find position along non-z axis of the plane.
      alongprofb = sqrt(interxb*interxb+interyb*interyb);//Use the values of x and y as well as pythagoras to find position along non-z axis of the plane.
   }
   return alongprofa > alongprofb;
}

void Profile::make_moving_average(){
   for(int i=0;i<linezsize;i++){
      delete[] linez[i];
   }
   delete[] linez;
   linezsize = flightlinestot.size();
   linez = new double*[linezsize];
   for(int i=0;i<linezsize;i++){
      int numofpoints = (int)flightlinepoints[i].size();
      linez[i] = new double[numofpoints];
      for(int j=0;j<numofpoints;j++){
         double z=0,zcount=0;
         for(int k=-mavrgrange;k<=mavrgrange;k++)if(j+k>=0&&j+k<numofpoints){// (up to) the range (depending on how close to the edge the point is) add up points...
            z+=flightlinepoints[i][j+k]->z;
            zcount++;
         }
         z /= zcount;//... and divide by the number of them to get the moving average at that point.
         linez[i][j]=z;
      }
   }
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
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   double red,green,blue;
   double x=0,y=0,z=0;
   int limit = bucketlimit;
   for(int i=0;i<(int)flightlinestot.size();i++){
      if((int)flightlinepoints[i].size()>limit)limit = (int)flightlinepoints[i].size();
   }
   float* vertices = new float[3*limit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*limit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   if(false/*drawpoints*/){
      int line=0,intensity=0,classification=0,rnumber=0;
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
               vertices[3*count]=x-centrex;
               vertices[3*count+1]=y-centrey;
               vertices[3*count+2]=z-centrez;
               colours[3*count]=red;
               colours[3*count+1]=green;
               colours[3*count+2]=blue;
               count++;
            }
         }
         glDrawArrays(GL_POINTS,0,count);
         if(numbuckets>90)if((i+1)%90==0){
            if (glwindow->is_double_buffered())glwindow->swap_buffers();//Draw to screen every bucket to show user stuff is happening.
            else glFlush();
         }
      }
   }
   if(rulering)makerulerbox();//Draw the ruler if ruler mode is on.
   if(true/*drawmovingaverage*/){
      int line=0,intensity=0,classification=0,rnumber=0;
      point *leftpnt = new point;
      leftpnt->x = leftboundx + centrex;
      leftpnt->y = leftboundy + centrey;
      leftpnt->z = 0;
      leftpnt->time = flightlinepoints[0][0]->time;
      leftpnt->intensity = 0;
      leftpnt->classification = 0;
      leftpnt->flightline = 0;
      leftpnt->rnumber = 0;
      point *rightpnt = new point;
      rightpnt->x = rightboundx + centrex;
      rightpnt->y = rightboundy + centrey;
      rightpnt->z = 0;
      rightpnt->time = flightlinepoints[0][0]->time;
      rightpnt->intensity = 0;
      rightpnt->classification = 0;
      rightpnt->flightline = 0;
      rightpnt->rnumber = 0;
      for(int i=0;i<(int)flightlinestot.size();i++){
         double tempx = minplanx,tempy= minplany;
         minplanx = startx + leftboundx;
         minplany = starty + leftboundy;
         int startindex = get_closest_element_position(rightpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
         int endindex = get_closest_element_position(leftpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
         minplanx = tempx;
         minplany = tempy;
         if(endindex < (int)flightlinepoints[i].size()-1)endindex++;
//         if(endindex < flightlinepoints[i].size()-1)endindex++;
         int count = 0;
         if(drawmovingaverage){
            int index = flightlinestot.at(i) % 6;
            switch(index){
               case 0:red=0;green=1;blue=0;break;//Green
               case 1:red=0;green=0;blue=1;break;//Blue
               case 2:red=1;green=0;blue=0;break;//Red
               case 3:red=0;green=1;blue=1;break;//Cyan
               case 4:red=1;green=1;blue=0;break;//Yellow
               case 5:red=1;green=0;blue=1;break;//Purple
               default:red=green=blue=1;break;//White in the event of strangeness.
            }
            for(int j=startindex;j<=endindex;j++){
               x = flightlinepoints[i][j]->x;
               y = flightlinepoints[i][j]->y;
               vertices[3*count]=x-centrex;
               vertices[3*count+1]=y-centrey;
               vertices[3*count+2]=linez[i][j]-centrez;
               colours[3*count]=red;
               colours[3*count+1]=green;
               colours[3*count+2]=blue;
               count++;
            }
            glDrawArrays(GL_LINE_STRIP,0,count);
         }
         count = 0;
         if(drawpoints){
            for(int j=startindex;j<=endindex;j++){
               red = 0.0; green = 1.0; blue = 0.0;//Default colour.
               x = flightlinepoints[i][j]->x;
               y = flightlinepoints[i][j]->y;
               z = flightlinepoints[i][j]->z;
               intensity = flightlinepoints[i][j]->intensity;
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
                   line = flightlinepoints[i][j]->flightline;
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
                   classification = flightlinepoints[i][j]->classification;
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
                   rnumber = flightlinepoints[i][j]->rnumber;
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
               vertices[3*count]=x-centrex;
               vertices[3*count+1]=y-centrey;
               vertices[3*count+2]=z-centrez;
               colours[3*count]=red;
               colours[3*count+1]=green;
               colours[3*count+2]=blue;
               count++;
            }
            glDrawArrays(GL_POINTS,0,count);
         }
      }
      delete leftpnt;
      delete rightpnt;
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
   int limit = bucketlimit;
   for(int i=0;i<(int)flightlinestot.size();i++){
      if((int)flightlinepoints[i].size()>limit)limit = (int)flightlinepoints[i].size();
   }
   float* vertices = new float[3*limit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*limit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   if(false)for(int i=0;i<numbuckets;i++){//For every bucket...
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
            vertices[3*count]=x-centrex;
            vertices[3*count+1]=y-centrey;
            vertices[3*count+2]=z-centrez;
            colours[3*count]=red;
            colours[3*count+1]=green;
            colours[3*count+2]=blue;
            count++;
         }
      }
      glDrawArrays(GL_POINTS,0,count);
   }
   if(true/*drawmovingaverage*/){
      int line=0,intensity=0,classification=0,rnumber=0;
      point *leftpnt = new point;
      leftpnt->x = leftboundx + centrex;
      leftpnt->y = leftboundy + centrey;
      leftpnt->z = 0;
      leftpnt->time = flightlinepoints[0][0]->time;
      leftpnt->intensity = 0;
      leftpnt->classification = 0;
      leftpnt->flightline = 0;
      leftpnt->rnumber = 0;
      point *rightpnt = new point;
      rightpnt->x = rightboundx + centrex;
      rightpnt->y = rightboundy + centrey;
      rightpnt->z = 0;
      rightpnt->time = flightlinepoints[0][0]->time;
      rightpnt->intensity = 0;
      rightpnt->classification = 0;
      rightpnt->flightline = 0;
      rightpnt->rnumber = 0;
      for(int i=0;i<(int)flightlinestot.size();i++){
         double tempx = minplanx,tempy= minplany;
         minplanx = startx + leftboundx;
         minplany = starty + leftboundy;
         int startindex = get_closest_element_position(rightpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
         int endindex = get_closest_element_position(leftpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
         minplanx = tempx;
         minplany = tempy;
         if(endindex < (int)flightlinepoints[i].size()-1)endindex++;
//         if(endindex < flightlinepoints[i].size()-1)endindex++;
         int count = 0;
         if(drawmovingaverage){
            int index = flightlinestot.at(i) % 6;
            switch(index){
               case 0:red=0;green=1;blue=0;break;//Green
               case 1:red=0;green=0;blue=1;break;//Blue
               case 2:red=1;green=0;blue=0;break;//Red
               case 3:red=0;green=1;blue=1;break;//Cyan
               case 4:red=1;green=1;blue=0;break;//Yellow
               case 5:red=1;green=0;blue=1;break;//Purple
               default:red=green=blue=1;break;//White in the event of strangeness.
            }
            for(int j=startindex;j<=endindex;j+=detail){
               x = flightlinepoints[i][j]->x;
               y = flightlinepoints[i][j]->y;
               vertices[3*count]=x-centrex;
               vertices[3*count+1]=y-centrey;
               vertices[3*count+2]=linez[i][j]-centrez;
               colours[3*count]=red;
               colours[3*count+1]=green;
               colours[3*count+2]=blue;
               count++;
            }
            glDrawArrays(GL_LINE_STRIP,0,count);
         }
         count = 0;
         if(drawpoints){
            for(int j=startindex;j<=endindex;j+=detail){
               red = 0.0; green = 1.0; blue = 0.0;//Default colour.
               x = flightlinepoints[i][j]->x;
               y = flightlinepoints[i][j]->y;
               z = flightlinepoints[i][j]->z;
               intensity = flightlinepoints[i][j]->intensity;
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
                   line = flightlinepoints[i][j]->flightline;
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
                   classification = flightlinepoints[i][j]->classification;
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
                   rnumber = flightlinepoints[i][j]->rnumber;
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
               vertices[3*count]=x-centrex;
               vertices[3*count+1]=y-centrey;
               vertices[3*count+2]=z-centrez;
               colours[3*count]=red;
               colours[3*count+1]=green;
               colours[3*count+2]=blue;
               count++;
            }
            glDrawArrays(GL_POINTS,0,count);
         }
      }
      delete leftpnt;
      delete rightpnt;
   }
   if(rulering)glCallList(5);//Draw the ruler if ruler mode is on.
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}
