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
#include "pointbucket.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Profile.h"
#include "MathFuncs.h"

Profile::Profile(const Glib::RefPtr<const Gdk::GL::Config>& config,quadtree* lidardata,int bucketlimit,Gtk::Label *rulerlabel)  : Display(config,lidardata,bucketlimit){
   samplemaxz = sampleminz = 0;
   viewerz = 0;
   startx = 0;
   starty = 0;
   showheightscale = false;
   //Initialisation:
   flightlinepoints = NULL;
   linez = NULL;
   //Drawing control:
   zoompower = 0.7;
   imageexists=false;
   drawpoints = true;
   drawmovingaverage = false;
   maindetailmod = 0;
   previewdetailmod = 0.3;
   mavrgrange = 5;
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

//This is called by a "reset button". It returns the view to the initial one. It sets the centre of the screen to the centre of the profile (average in the case of Z) and then sets the viewer position and the ratio of world coordinates to window coordinates before resetting the view and then drawing.
bool Profile::returntostart(){
   centrex = (startx + endx)/2;//This way, all of the profile should be on-screen.
   centrey = (starty + endy)/2;//...
   centrez = (samplemaxz + sampleminz)/2;
//   centrez=0;
//   for(int i=0;i<(int)flightlinestot.size();i++){//Totalling z...
//      centrez+=flightlinepoints[i][0].z;
//      centrez+=flightlinepoints[i][flightlinepoints[i].size()-1].z;
//   }
//   centrez/=(2*flightlinestot.size());//Dividing to get average.
   zoomlevel=1;
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   viewerx = width * height / length;//To the right when looking from start to end.
   viewery = -width * breadth / length;//...
   ratio = length/get_width();
   double Z = samplemaxz - sampleminz;
   if(ratio<Z/get_height())ratio = Z/get_height();
   ratio*=1.1;
   resetview();
   return drawviewable(1);
}

//This method accepts the parameters of the profile and gets the data from the quadtree. It then determines which points from the returned buckets are actually within the boundaries of the profile at the same time as determining how many and what flightlines there are. It then creates a new pointer to an array of vectors, each vector being for each flightline and containing all the points from that flightline that are also withing the boundaries of the profile. It then sorts these points, in each flightline, so that meaningful moving averages can be made as well as quick searches along the data to show only the needed data on the screen. It then makes a moving average using the settings already existing and then draws.
bool Profile::showprofile(double* profxs,double* profys,int profps){
   startx = (profxs[0]+profxs[1])/2;
   starty = (profys[0]+profys[1])/2;
   endx = (profxs[profps-1]+profxs[profps-2])/2;
   endy = (profys[profps-1]+profys[profps-2])/2;
   width = sqrt((profxs[0]-profxs[1])*(profxs[0]-profxs[1])+(profys[0]-profys[1])*(profys[0]-profys[1]));
   vector<pointbucket*> *pointvector;
   try{
      pointvector = lidardata->advsubset(profxs,profys,profps);//Get data.
      imageexists=true;
   }catch(descriptiveexception e){
      cout << "There has been an exception:" << endl;
      cout << "What: " << e.what() << endl;
      cout << "Why: " << e.why() << endl;
      cout << "No points returned." << endl;
      imageexists=false;
      return false;
   }
   if(pointvector==NULL||pointvector->size()==0){
      imageexists=false;
      return false;
   }
   int numbuckets = pointvector->size();
   flightlinestot.clear();
   bool** correctpointsbuckets = new bool*[numbuckets];//This stores, for each point in each bucket, whether the point is inside the boundaries of the profile and, therefore, whether the point should be drawn.
   for(int i=0;i<numbuckets;i++){//Convert to pointer for faster access in for loops in image methods. Why? Expect >100000 points.
      correctpointsbuckets[i] = vetpoints((*pointvector)[i],profxs,profys,profps);
      for(int j=0;j<(*pointvector)[i]->getnumberofpoints();j++){
         if(correctpointsbuckets[i][j]){//This gets from all the points their flightline numbers and compiles a list of all the flightlines in the profile.
            if(find(flightlinestot.begin(),flightlinestot.end(),(*pointvector)[i]->getpoint(j).flightline)==flightlinestot.end()){
               flightlinestot.push_back((*pointvector)[i]->getpoint(j).flightline);
            }
         }
      }
   }
   if(flightlinepoints!=NULL)delete[] flightlinepoints;
   flightlinepoints = new vector<point>[flightlinestot.size()];
   totnumpoints = 0;
   samplemaxz = rminz;
   sampleminz = rmaxz;
   for(int i=0;i<(int)flightlinestot.size();i++){
      for(int j=0;j<numbuckets;j++){//Get all points that should be accounted for:
         for(int k=0;k<(*pointvector)[j]->getnumberofpoints();k++){
            if(correctpointsbuckets[j][k]){
               if((*pointvector)[j]->getpoint(k).flightline == flightlinestot[i]){
                  flightlinepoints[i].push_back((*pointvector)[j]->getpoint(k));
                  totnumpoints++;
                  if(samplemaxz<(*pointvector)[j]->getpoint(k).z)samplemaxz = (*pointvector)[j]->getpoint(k).z;
                  if(sampleminz>(*pointvector)[j]->getpoint(k).z)sampleminz = (*pointvector)[j]->getpoint(k).z;
               }
            }
         }
      }
      minplanx = startx;//These are the initial values, as the initial position of the viewing area will be defiend by the start and end coordinates of the profile.
      minplany = starty;//...
      sort(flightlinepoints[i].begin(),flightlinepoints[i].end(),boost::bind(&Profile::linecomp,this,_1,_2));//Sort so that lines are intelligible and right.
   }
   make_moving_average();
   glViewport(0, 0, get_width(), get_height());
   get_gl_window()->make_current(get_gl_context());
   delete pointvector;
   for(int i=0;i<numbuckets;i++)delete[] correctpointsbuckets[i];
   delete[] correctpointsbuckets;
   if(is_realized())return returntostart();
   else return false;
}

//Firstly, this determines the boundary of the viewable area in world coordinates (for use by the drawing method(s)). It then sets the active matrix to that of projection and makes it the identity matrix, and then defines the limits of the viewing area from the dimensions of the window. *ratio*zoomlevel is there to convert screen dimensions to image dimensions. gluLookAt is then used so that the viewpoint is that of seeing the centre from a position to the right of the profile, when looking from the start to the end of it.
void Profile::resetview(){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   leftboundx = -((get_width()/2)*ratio/zoomlevel) * breadth / length;//This part determines the boundary coordinates in world coordinates.
   rightboundx = ((get_width()/2)*ratio/zoomlevel) * breadth / length;//...
   leftboundy = -((get_width()/2)*ratio/zoomlevel) * height / length;//...
   rightboundy = ((get_width()/2)*ratio/zoomlevel) * height / length;//...
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
}

//This returns the index (in the vector of points in a flightline) of the nearest point "before" the position along the horizontal line of the viewable plane of the point passed in. It is used for determining which points to draw by passing as a "point" the coordinates of the limits of the viewable plane. It needs to be passed a "point" because the linecomp function, which it uses, only accepts points because it was originally made just for sorting points. Fundamentally, it works similarly to a BINARY SEARCH algorithm.
int Profile::get_closest_element_position(point* value,vector<point>::iterator first,vector<point>::iterator last){
   vector<point>::iterator originalFirst = first;
   vector<point>::iterator middle;
   while(true){//INFINITE LOOP interrupted by returns.
      middle = first + distance(first,last)/2;
      if(linecomp(*middle,*value))first = middle;//IF the passed point is further along the horizontal plane-line than the "middle" point then make the "first" point equal to the "middle" point.
      else if(linecomp(*value,*middle))last = middle;//ELSE IF the opposite, make the "last" point equal to the "middle" point.
      else return distance(originalFirst,middle);//ELSE, in the very rare event that the passed point is exactly equal in x and y coordinates (or, more correctly, its "hypotenuse is equal, as that can happen with the coordinates being different) return the position in the vector where that happens.
      if(distance(first,last)<2 && distance(first,middle)<1)return distance(originalFirst,middle);//IF the "first" and "middle" are now in the same position AND the distance between first and last is now just 1, as will (almost, see above line) inevitably happen because of the properties of integer division (at the beginning of the loop), then return the distane between the original "first" and the current "middle", as this is the point that most closely approximates the position along the vector (and HORIZONTALLY across the plane) of the point passed in.
   }
}

//Depending on the imagetype requested, this sets the detail level and then calls one of the image methods, which actually draws the data to the screen. The passed value should be 1 for the main image, 2 for the preview and 3 for the expose event (which is the same as the preview).
bool Profile::drawviewable(int imagetype){
   if(!imageexists){//If there is an attempt to draw with no data, the program will probably crash.
      Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
      if (!glwindow->gl_begin(get_gl_context()))return false;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//No data in store, no data on screen.
      if (glwindow->is_double_buffered())glwindow->swap_buffers();
      else glFlush();
      return false;
   }
   glPointSize(pointsize);//These are here to prevent interference from the overview. The overview has similar protections.
   glViewport(0, 0, get_width(), get_height());//...
   get_gl_window()->make_current(get_gl_context());//...
   resetview();//...
   int detail=1;//This determines how many points are skipped between reads.
   //If there are very few points on the screen, show them all:
   if(imagetype==1)detail=(int)(totnumpoints*maindetailmod/100000);//Main image.  
   else if(imagetype==2||imagetype==3)detail=(int)(totnumpoints*previewdetailmod/100000);//Preview
   if(detail<1)detail=1;
   mainimage(detail);
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
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = (event->x-panstartx)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
   centrex -= hypotenuse * breadth / length;
   centrey -= hypotenuse * height / length;
   centrez += (event->y-panstarty)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   panstartx=event->x;
   panstarty=event->y;
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
   double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
   rulerstartx = rulerendx = centrex + viewerx + hypotenuse * breadth / length;
   rulerstarty = rulerendy = centrey + viewery + hypotenuse * height / length;
   rulerstartz = rulerendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   ostringstream zpos;
   zpos << rulerendz;
   rulerlabel->set_text("Distance: 0\nX: 0\nY: 0\nHoriz: 0\nZ: 0 Pos: " + zpos.str());
   return drawviewable(1);
}
//Find the current cursor coordinates in image terms (as opposed to window/screen terms) and then update the label with the distances. Then draw the ruler.
bool Profile::on_ruler(GdkEventMotion* event){
   double breadth = endx - startx;
   double height = endy - starty;
   double length = sqrt(breadth*breadth+height*height);//Right triangle.
   double hypotenuse = (event->x-get_width()/2)*ratio/zoomlevel;//The horizontal distance is a combination of x and y so:
   rulerendx = centrex + viewerx + hypotenuse * breadth / length;
   rulerendy = centrey + viewery + hypotenuse * height / length;
   rulerendz = centrez + viewerz - (event->y-get_height()/2)*ratio/zoomlevel;//Z is reversed because gtk has origin at top left and opengl has it at bottom left.
   double d,xd,yd,hd,zd;
   xd = abs(rulerendx-rulerstartx);
   yd = abs(rulerendy-rulerstarty);
   zd = abs(rulerendz-rulerstartz);
   hd = sqrt(xd*xd+yd*yd);//Combined horizontal distance.
   d = sqrt(hd*hd+zd*zd);//Combined horizontal and vertical distance.
   ostringstream dist,xdist,ydist,horizdist,zdist,zpos;
   dist << d;
   xdist << xd;
   ydist << yd;
   horizdist << hd;
   zdist << zd;
   zpos << rulerendz;
   string rulerstring = "Distance: " + dist.str() + "\nX: " + xdist.str() + "\nY: " + ydist.str() + "\nHoriz: " + horizdist.str() + "\nZ: " + zdist.str() + " Pos: " + zpos.str();
   rulerlabel->set_text(rulerstring);
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
void Profile::drawoverlays(){
   if(rulering)makerulerbox();
   if(showheightscale)makeZscale();
}

//This draws a scale. It works out what order of magnitude to use for the scale and the number of intervals to have in it and then modifies these if there would be too few or too mant intervals. It then draws the vertical line and the small horizontal markers before setting up the font settings and then drawing the numbers by the markers.
void Profile::makeZscale(){
   double rheight = get_height()*ratio/zoomlevel;
   double order=1;
   if(rheight>5)for(int i=rheight;i>10;i/=10)if(rheight/(order*10)>5)order*=10;//This finds the order of magnitude (base 10) of rheight with the added proviso that rheight must be at least five times that order so that there are enough intervals to draw a decent scale. This gives a range of nummarks values (below) of 5-50. While it may seem that the i variable could be used instead of rheight/(order*10), this is not the case as the latter is a double calculationi, while the former is a result of a series of integer calculations, so the results diverge.
   if(rheight<=5)for(double i=rheight;i<10;i*=10)order/=10;//For when the user zooms really far in.
   int nummarks = (int)(0.9*rheight/order);//Again, it would be tempting to use i here, but this is only one integer calculation while i is the result (probably) of several such calculations, and so has lost more precision.
   while(nummarks>10){//The original order we calculated would give a number of scale widths from 5-50, but anything more than 10 is probably too much, so this loop doubles the order value until nummarks falls below 10.
      order*=2;
      nummarks = (int)(0.9*rheight/order);
   }
   double padding = (rheight - nummarks*order)/2;//It would be more aesthetically pleasing to centre the scale.
   GLint viewport[4];
   GLdouble modelview[16];
   GLdouble projection[16];
   GLdouble origx,origy,origz;//The world coordinates of the origin for the screen coordinates.
   glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
   glGetDoublev(GL_PROJECTION_MATRIX,projection);
   glGetIntegerv(GL_VIEWPORT,viewport);
   gluUnProject(50,0,0.1,modelview,projection,viewport,&origx,&origy,&origz);
   GLdouble origx2,origy2,origz2;
   gluUnProject(80,0,0.1,modelview,projection,viewport,&origx2,&origy2,&origz2);
   GLdouble origx3,origy3,origz3;
   gluUnProject(85,0,0.1,modelview,projection,viewport,&origx3,&origy3,&origz3);//DAMN! Precision problems when zoomed right in! This is because all OpenGL functions on most graphics cards use floats only, not doubles. Might want to replace this with my own method (!!!).
   glColor3f(1.0,1.0,1.0);
   glBegin(GL_LINES);
      glVertex3d(origx,origy,origz + padding);//Vertical line.
      glVertex3d(origx,origy,origz + padding + nummarks*order);//...
      for(int i=0;i<=nummarks;i++){//Horizontal lines.
         glVertex3d(origx,origy,origz + padding + i*order);
         glVertex3d(origx2,origy2,origz2 + padding + i*order);
      }
   glEnd();
   GLuint fontlists = glGenLists(128);//ASCII!
   Pango::FontDescription font_desc("courier 12");
   Glib::RefPtr<Pango::Font> font = Gdk::GL::Font::use_pango_font(font_desc,0,128,fontlists);//Make a selection of letters and numbers for use below (though we only use the numbers).
   if(!font)cerr << "Cannot load font!" << endl;//Trouble at t'mill! One of t'crossbeam's g'nout of skew 'nt'treadle!
   for(int i=0;i<=nummarks;i++){
      glRasterPos3d(origx3,origy3,origz3 + padding + i*order);//Draw numbers by the horizontal lines.
      ostringstream number;
      number << origz3 + centrez + i*order + padding;
      glListBase(fontlists);
      glCallLists(number.str().length(),GL_UNSIGNED_BYTE,number.str().c_str());
   }
}
   
//First, the distance between the centre of the window and the window position of the event is converted to image coordinates and added to the image centre. This is analogous to moving the centre to where the event occured. Then, depending on the direction of the scroll, the zoomlevel is increased or decreased. Then the centre is moved to where the centre of the window will now lie. The image is then drawn.
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

//This method is used by sort() and get_closest_element_position(). It projects the points onto a plane defined by the z axis and the other line perpendicular to the viewing direction. It then returns whether the first point is "further along" the plane than the second one, with one of the edges of the plane being defined as that "start".
bool Profile::linecomp(const point &a,const point &b){
   const double xa = a.x;
   const double xb = b.x;
   const double ya = a.y;
   const double yb = b.y;
   double alongprofa,alongprofb;
   if(startx==endx){//If the profile is parallel to the y axis:
      double mult=-1;//Used so that points are projecting onto the correct side (NOT face) of the plane.
      if(starty<endy)mult=1;
      alongprofa = mult * (ya - minplany);
      alongprofb = mult * (yb - minplany);
   }
   else if(starty==endy){//If the profile is parallel to the x axis:
      double mult=-1;//Used so that points are projecting onto the correct side (NOT face) of the plane.
      if(startx<endx)mult=1;
      alongprofa = mult * (xa - minplanx);
      alongprofb = mult * (xb - minplanx);
   }
   else{//If the profile is skewed:
      double breadth = endx - startx;
      double height = endy - starty;
      double multx=-1;//Used so that points are projecting onto the correct side (NOT face) of the plane.
      if(startx<endx)multx=1;
      double multy=-1;//Used so that points are projecting onto the correct side (NOT face) of the plane.
      if(starty<endy)multy=1;
      //Gradients of the profile and point-to-profile lines:
      double lengradbox = multx * multy * height / breadth;//Profile line
      double widgradbox = -1.0 / lengradbox;//Point-to-profile lines
      //Constant values (y intercepts) of the formulae for lines from each point to the profile line:
      double widgradboxa = multy * (ya - minplany) - (multx * (xa - minplanx) * widgradbox);
      double widgradboxb = multy * (yb - minplany) - (multx * (xb - minplanx) * widgradbox);
      //Identify the points of interecept for each point-to-profile line and the profile line:
      /*0 (adjusted origin)
       * \ Profile line       ____/p
       *  \              ____/ Point line
       *   \        ____/
       *    \  ____/
       *  ___\/P
       * /    \
       *       \
       *        \
       *                              
       *  For point p:
       *     x of P is interxp
       *     y of P is interyp
       *     z is ignored (or "swept along")
       *     alongprofp is sqrt(interxp^2 + interyp^2), i.e. Pythagoras to find distance along the profile i.e distance from the adjusted origin.
       *
       * */
      double interxa,interxb,interya,interyb;
      interxa = widgradboxa / (widgradbox - lengradbox);//The x (intercept with plane) value of the line from the point a to the plane.
      interya = interxa * lengradbox;//The y (intercept with plane) value of the line from the point a to the plane.
      interxb = widgradboxb / (widgradbox - lengradbox);//The x (intercept with plane) value of the line from the point b to the plane.
      interyb = interxb * lengradbox;//The y (intercept with plane) value of the line from the point b to the plane.
      alongprofa = sqrt(interxa*interxa+interya*interya);//Use the values of x and y as well as pythagoras to find position along non-z axis of the plane.
      alongprofb = sqrt(interxb*interxb+interyb*interyb);//Use the values of x and y as well as pythagoras to find position along non-z axis of the plane.
   }
   return alongprofa > alongprofb;
}

//This creates an array of z values for the points in the profile that are derived from the real z values through a moving average. This results in a smoothed line.
void Profile::make_moving_average(){
   if(linez!=NULL){
      for(int i=0;i<linezsize;i++){
         delete[] linez[i];
      }
      delete[] linez;
   }
   linezsize = flightlinestot.size();
   linez = new double*[linezsize];
   for(int i=0;i<linezsize;i++){
      int numofpoints = (int)flightlinepoints[i].size();
      linez[i] = new double[numofpoints];
      for(int j=0;j<numofpoints;j++){
         double z=0,zcount=0;
         for(int k=-mavrgrange;k<=mavrgrange;k++)if(j+k>=0&&j+k<numofpoints){//for (up to) the range (depending on how close to the edge the point is) add up the points...
            z+=flightlinepoints[i][j+k].z;
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
bool Profile::mainimage(int detail){
   Glib::RefPtr<Gdk::GL::Window> glwindow = get_gl_window();
   if (!glwindow->gl_begin(get_gl_context()))return false;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Need to clear screen because of gaps.
   double red,green,blue,z;
   int intensity;
   int limit = 0;
   for(int i=0;i<(int)flightlinestot.size();i++){//The size of the vertex and colour arrays should be the same as that of the largest group of points, by flightline.
      if((int)flightlinepoints[i].size()>limit)limit = (int)flightlinepoints[i].size();
   }
   float* vertices = new float[3*limit];//Needed for the glDrawArrays() call further down.
   float* colours = new float[3*limit];//...
   glEnableClientState(GL_VERTEX_ARRAY);//...
   glEnableClientState(GL_COLOR_ARRAY);//...
   glVertexPointer(3, GL_FLOAT, 0, vertices);//...
   glColorPointer(3, GL_FLOAT, 0, colours);//...
   point *leftpnt = new point;//Fake point for sending to linecomp and get_closest_element_position the boundaries of the screen.
   leftpnt->x = leftboundx + centrex;
   leftpnt->y = leftboundy + centrey;
   leftpnt->z = 0;
   leftpnt->time = flightlinepoints[0][0].time;
   leftpnt->intensity = 0;
   leftpnt->classification = 0;
   leftpnt->flightline = 0;
   leftpnt->packedbyte = 0;
   point *rightpnt = new point;//Fake point for sending to linecomp and get_closest_element_position the boundaries of the screen.
   rightpnt->x = rightboundx + centrex;
   rightpnt->y = rightboundy + centrey;
   rightpnt->z = 0;
   rightpnt->time = flightlinepoints[0][0].time;
   rightpnt->intensity = 0;
   rightpnt->classification = 0;
   rightpnt->flightline = 0;
   rightpnt->packedbyte = 0;
   for(int i=0;i<(int)flightlinestot.size();i++){
      minplanx = startx + leftboundx;//These ensure that the entire screen will be filled, otherwise, because the screen position of startx changes, only part of the point-set will be drawn.
      minplany = starty + leftboundy;//...
      int startindex = get_closest_element_position(rightpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
      int endindex = get_closest_element_position(leftpnt,flightlinepoints[i].begin(),flightlinepoints[i].end());
      for(int l=0;l<detail*2;l++)if(endindex < (int)flightlinepoints[i].size()-1)endindex++;//This is to ensure that the left edge of the screen always has a line crossing it if there are extra points beyond it, otherwise it will only draw up to, not beyond, the last point actually on-screen.
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
            vertices[3*count] = flightlinepoints[i][j].x-centrex;
            vertices[3*count+1] = flightlinepoints[i][j].y-centrey;
            vertices[3*count+2] = linez[i][j]-centrez;
            colours[3*count] = red;
            colours[3*count+1] = green;
            colours[3*count+2] = blue;
            count++;
         }
         glDrawArrays(GL_LINE_STRIP,0,count);//Send contents of arrays to OpenGL, ready to be drawn when the buffer is flushed.
      }
      count = 0;
      if(drawpoints){
         for(int j=startindex;j<=endindex;j+=detail){
            red = 0.0; green = 1.0; blue = 0.0;//Default colour.
            z = flightlinepoints[i][j].z;//This is here because it is used in calculations.
            intensity = flightlinepoints[i][j].intensity;
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
                int index = flightlinepoints[i][j].flightline % 6;
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
                switch(flightlinepoints[i][j].classification){
                   case 0:case 1:red=1;green=1;blue=0;break;//Yellow for non-classified.
                   case 2:red=0.6;green=0.3;blue=0;break;//Brown for ground.
                   case 3:red=0;green=0.3;blue=0;break;//Dark green for low vegetation.
                   case 4:red=0;green=0.6;blue=0;break;//Medium green for medium vegetation.
                   case 5:red=0;green=1;blue=0;break;//Bright green for high vegetation.
                   case 6:red=0;green=1;blue=0;break;//Cyan for buildings.
                   case 7:red=1;green=0;blue=1;break;//Purple for low point (noise).
                   case 8:red=0.5;green=0.5;blue=0.5;break;//Grey for model key-point (mass point).
                   case 9:red=0;green=0;blue=1;break;//Blue for water.
                   case 12:red=1;green=1;blue=1;break;//White for overlap points.
                   default:red=1;green=0;blue=0;cout << "Undefined point." << endl;break;//Red for undefined.
                }
            }
            else if(returncolour){//Colour by return.
                switch(flightlinepoints[i][j].packedbyte & returnnumber){
                   case 1:red=0;green=0;blue=1;break;//Blue
                   case 2:red=0;green=1;blue=1;break;//Cyan
                   case 3:red=0;green=1;blue=0;break;//Green
                   case 4:red=1;green=0;blue=0;break;//Red
                   case 5:red=1;green=0;blue=1;break;//Purple
                   default:red=green=blue=1;break;//White in the event of strangeness.
                }
            }
            if(heightbrightness){//Shade by height.
               red *= brightnessheightarray[(int)(10*(z-rminz))];
               green *= brightnessheightarray[(int)(10*(z-rminz))];
               blue *= brightnessheightarray[(int)(10*(z-rminz))];
            }
            else if(intensitybrightness){//Shade by intensity.
               red *= brightnessintensityarray[(int)(intensity-rminintensity)];
               green *= brightnessintensityarray[(int)(intensity-rminintensity)];
               blue *= brightnessintensityarray[(int)(intensity-rminintensity)];
            }
            vertices[3*count] = flightlinepoints[i][j].x-centrex;
            vertices[3*count+1] = flightlinepoints[i][j].y-centrey;
            vertices[3*count+2]=z-centrez;
            colours[3*count]=red;
            colours[3*count+1]=green;
            colours[3*count+2]=blue;
            count++;
         }
         glDrawArrays(GL_POINTS,0,count);//Send contents of arrays to OpenGL, ready to be drawn when the buffer is flushed.
      }
   }
   delete leftpnt;
   delete rightpnt;
   drawoverlays();
   if (glwindow->is_double_buffered())glwindow->swap_buffers();
   else glFlush();
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   glwindow->gl_end();
   delete[] vertices;
   delete[] colours;
   return true;
}
