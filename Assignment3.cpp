// 	Item 2: created GIF successfully? Yes
// 	Item 3: created GIF successfully? Yes
// 	Item 4: created GIF successfully? Yes
// 	Item 5: created GIF successfully? Yes
// 	Item 6: created GIF successfully? Yes
// 	Item 13: created animated GIF of Scenery successfully? Yes
// 	Item 16: created the animated GIF of random images successfully? Yes
//	Item 17: Web page launches successfully from program? Yes

//	Any other comments?


// 816039130-COMP1602-A3
#include <iostream>
#include<cstdlib>
#include <fstream>
#include <cstring>
#include<cmath>
#include <ctime>
#include <windows.h>
using namespace std;

struct Image { 		// stores the bytes of the image
	unsigned char descriptor [10];
	unsigned char colourTable [768];
	unsigned char data [10000000];
	int sizeData;
}; 	


struct AnimFrame {	// stores each image/frame of the animation and the display time
	Image * image;
	int duration;
};

struct Animation {	// stores all the frames of the animation
	AnimFrame frames[20];
	int numFrames;
};


Image * readImage (char filename[]); // creates an image struct from the GIF file 
void saveImage (Image * image, char filename[]); // creates a GIF file from an image struct
Image * copyImage (Image * image); // creates a copy of the image struct
Image * applyEffectImage (Image * image, string effect); // creates a new image struct and then applies effect to it

Animation * initAnimation (); // creates a new animation with no frames
void addFrame (Animation * animation, Image * image, int duration); // creates a frame and adds it to the end of the animation
void saveAnimation (Animation * animation, char filename[]); // creates a GIF file from the animation


Image * readImage (char filename[]) {
	Image * image;
	unsigned char byte;
	ifstream in;
	int numBytes=0;
	
	image = new Image;
	
	in.open(filename, ios::in | ios::binary);
	if(!in.is_open()){
		cout << "Error opening file";
		return NULL;
	}
	// ignore first 13 bytes in image file
	long location = 13;
	in.seekg(location, ios::beg);
	
	// read 10 bytes from image file to descriptor field 
	in.read((char*) &(image->descriptor), sizeof(image->descriptor));
	
	// read 768 bytes from image file to colourTable field
	in.read((char*) &(image->colourTable), sizeof(image->colourTable));
	
	// read remaining bytes from image file into data field
	in.read((char*) &byte, sizeof(char));
	while(!in.eof()){
		image->data[numBytes] = byte;
		numBytes++;
		in.read((char*) &byte, sizeof(char));
	}
	
	image->sizeData = numBytes-1;
	return image;	
}



void saveImage (Image * image, char filename[]) {

	unsigned char endByte = 59;
	unsigned char header[6]= {71, 73, 70, 56, 57, 97};
	unsigned char lsd[7]= {88, 2, 144, 1, 112, 0, 0};
	
	ofstream out;
	out.open(filename, ios::binary | ios::out);
	if(!out.is_open()){
		cout << "Error opening file";
		return;
	}

	// write bytes from header
	
	out.write((char*) &header, sizeof(header));
	
	// write bytes from logical screen desciptor 
	out.write((char*) &lsd, sizeof(lsd));
	
	// write bytes from descriptor field
	
	out.write((char*) &image->descriptor, sizeof(image->descriptor));

	// write bytes from colourTable
	out.write((char*) &image->colourTable, sizeof(image->colourTable));
	
	// write sizeData bytes from data field

	for(int i=0;i<image->sizeData;i++)
	{
		out.write((char*) &image->data[i], sizeof(char));	
	}
	
	// write terminating byte
	out.write((char*) &endByte, sizeof(char));
}



Image * copyImage (Image * image) {
	Image * newImage;

	newImage = new Image;
	
	 //copy the descriptor

	 for (int i = 0; i < 10; i++)
        newImage->descriptor[i] = image->descriptor[i];
	
	 //copy the colourTable
	
	for (int i = 0; i < 768; i++)
        newImage->colourTable[i] = image->colourTable[i];
	
	 //copy the data
	
	for (int i = 0; i < image->sizeData; i++)
        newImage->data[i] = image->data[i];
	
	newImage->sizeData = image->sizeData;
	
	return newImage;
}



void toGrayScale (int * red, int * green, int * blue) {
	int sum = *red + (*green) + (*blue);
	int avg = sum / 3;
	
	*red = avg;
	*blue = avg;
	*green = avg;
}



void toBlueTint (int * red, int * green, int * blue) {
	int sum = *red + (*green) + (*blue);
	int avg = sum / 3;
	
	*red = avg;
	*green = avg;

}



void toRedTint (int * red, int * green, int * blue) {
	int sum = *red + (*green) + (*blue);
	int avg = sum / 3;
	
	*blue = avg;
	*green = avg;

}



void toSepia (int * red, int * green, int * blue) {
	
	int newRed, newGreen, newBlue;
	newRed = *red * 0.393 + *green * 0.769 + *blue * 0.189;
	newGreen = *red * 0.349 + *green * 0.686 + *blue * 0.168;
	newBlue = *red * 0.272 + *green * 0.534 + *blue * 0.131;
	if(newRed > 255)
		newRed=255;
	if(newGreen > 255)
		newGreen=255;
	if(newBlue > 255)
		newBlue=255;
		
	*red = newRed;
	*blue = newBlue;
	*green = newGreen;
}



void brighten (int * red, int * green, int * blue, int percent) {
	float perc = (100+100+percent)/100.0;
	*red *= perc;
	*green *= perc;
	*blue *= perc;
	
	if(*red > 255)
		*red = 255;
	if(*green > 255)
		*green = 255;
	if(*blue > 255)
		*blue = 255;

}



Image * applyEffectImage (Image * image, string effect) {
	Image * newImage;
	int *red, *green, *blue;
	red = new int;
	green = new int;
	blue = new int;
	int i=0;
	

	// copy image to newImage and apply effect to bytes in colourTable of newImage
	newImage = copyImage(image);
	int count = 0;
	while(i<=765)
	{
		int r = newImage->colourTable[i];
		int g = newImage->colourTable[i+1];
		int b = newImage->colourTable[i+2];
		*red = r;
		*green = g;
		*blue = b;
		if(effect == "grayscale")
		{
			toGrayScale(red, green, blue);
			newImage->colourTable[i]=*red;
			newImage->colourTable[i+1]=*green;
			newImage->colourTable[i+2]=*blue;
		}
		if(effect == "bluetint")
		{
			toBlueTint(red, green, blue);
			newImage->colourTable[i]=*red;
			newImage->colourTable[i+1]=*green;
		}
		if(effect == "redtint")
		{
			toRedTint(red, green, blue);
			newImage->colourTable[i+1]=*green;
			newImage->colourTable[i+2]=*blue;
		}
		if(effect == "sepia")
		{
			toSepia(red, green, blue);
			newImage->colourTable[i]=*red;
			newImage->colourTable[i+1]=*green;
			newImage->colourTable[i+2]=*blue;
		}
		if(effect == "brighten")
		{
			brighten(red, green, blue, 60);
			newImage->colourTable[i]=*red;
			newImage->colourTable[i+1]=*green;
			newImage->colourTable[i+2]=*blue;
		}
		
		i+=3;
	}
	delete(red);
	delete(green);
	delete(blue);
	return newImage;
}



Animation * initAnimation () {
	Animation * newAnim;
	
	newAnim = new Animation;
	
	newAnim->numFrames = 0;
	
	return newAnim;
}



void addFrame (Animation * animation, Image * image, int duration) {
	animation->frames[animation->numFrames].image = image;
	animation->frames[animation->numFrames].duration = duration;
	animation->numFrames++;
}
	


void saveAnimation (Animation * animation, char filename[]) {

	unsigned char endByte = 59;
	unsigned char lsd[7]= {88, 2, 144, 1, 112, 0, 0};
	unsigned char header[6]= {71, 73, 70, 56, 57, 97};
	unsigned char appExt[19]= {33, 255, 11, 78, 69, 84, 83, 67, 65, 80, 69, 50, 46, 48, 3, 1, 0, 0, 0};
	unsigned char gcExt[8]= {33, 249, 4, 8, 100, 0, 0, 0};
	
	ofstream out;
	out.open(filename, ios::binary | ios::out);
	if(!out.is_open())
	{
		cout << "Error opening file...";
		return;
	}
	// write bytes from header
	out.write((char*) &header, sizeof(header));
	
	// write bytes from lsd
	out.write((char*) &lsd, sizeof(lsd));
	
	// write bytes from application extension
	out.write((char*) &appExt, sizeof(appExt));
	
	// write bytes for each frame
	for(int i=0;i<animation->numFrames;i++)
	{
		gcExt[4] = animation->frames[i].duration;
		out.write((char*) &gcExt, sizeof(gcExt));
		out.write((char*) &animation->frames[i].image->descriptor, sizeof(animation->frames[i].image->descriptor));
		out.write((char*) &animation->frames[i].image->colourTable, sizeof(animation->frames[i].image->colourTable));
		for(int j=0;j<animation->frames[i].image->sizeData;j++)
			out.write((char*) &animation->frames[i].image->data[j], sizeof(char));
	}
	
	// write terminating byte
	out.write((char*) &endByte, sizeof(char));
} 



// This function displays a Web page on the default browser (Windows only)

void showWebPage (string URL) {
	char cURL [50];
	int i;

	for (i = 0; i < URL.length(); i=i+1){
    	cURL[i] = URL[i];
	}

	cURL[URL.length()] = '\0';
    
    ShellExecute(NULL, "open", cURL, NULL, NULL, SW_SHOWNORMAL);
}



int main() {

	srand(time(NULL));
	Animation * scenery;
	Animation * random;

	Image * img1;
	Image * img2;
	Image * img3;
	Image * img4;
	Image * img5;

	Image * imgEff1; 
	Image * imgEff2; 
	Image * imgEff3; 
	Image * imgEff4; 
	Image * imgEff5;
	
	char img1File[50] = "Saskatoon-GS.gif";
	char saskatoon[25] = "Saskatoon.gif";
	char img2File[50] = "MoraineLake-BT.gif";
	char moraineLake[25] = "MoraineLake.gif";
	char img3File[50] = "Maracas-S.gif";
	char maracas[25] = "Maracas.gif";
	char img4File[50] = "ScarletIbis-B.gif";
	char scarletIbis[25] = "ScarletIbis.gif";
	char img5File[50] = "PigeonPoint-RT.gif";
	char pigeonPoint[25] = "PigeonPoint.gif";
	
	char sceneryAnim[25] = "Scenery-Animation.gif";
	char randomAnim[25] = "Random-Animation.gif";
	
	char webPage[50];	
	strcpy (webPage, "Assignment3.html");	// copy Assignment3.html to folder with C++ program
	
	// Operation 1-create images for each GIF file
	img1=readImage(saskatoon);
	img2=readImage(moraineLake);
	img3=readImage(maracas);
	img4=readImage(scarletIbis);
	img5=readImage(pigeonPoint);
	

	// Operation 2 to 6-apply effects to images and save to new GIF files
	imgEff1=applyEffectImage(img1, "grayscale");
	imgEff2=applyEffectImage(img2, "bluetint");
	imgEff3=applyEffectImage(img3, "sepia");
	imgEff4=applyEffectImage(img4, "brighten");
	imgEff5=applyEffectImage(img5, "redtint");
	
	saveImage(imgEff1, img1File);
	saveImage(imgEff2, img2File);
	saveImage(imgEff3, img3File);
	saveImage(imgEff4, img4File);
	saveImage(imgEff5, img5File);
	
	// Operation 7-create scenery animation
	scenery= initAnimation();
	
	// Operation 8 to 12-add frames to scenery animation (250 duration for each frame)
	addFrame(scenery, img3, 250);
	addFrame(scenery, imgEff2, 250);
	addFrame(scenery, img5, 250);
	addFrame(scenery, imgEff3, 250);
	addFrame(scenery, img1, 250);
	

	// Operation 13-save scenery animation to GIF file
	saveAnimation(scenery, sceneryAnim);

	// Operation 14-create random animation
	random = initAnimation();
	
	// Operation 15-add random frames to random animation (random duration between 50 and 255)
	for(int i=0;i<7;i++)
	{
		int randomNum = rand() % 10 + 1;
		int randTime = 50 + rand() % (255-50+1);
		if(randomNum == 1)
			addFrame(random, img1, randTime);
		else if(randomNum == 2)
			addFrame(random, img2, randTime);
		else if(randomNum == 3)
			addFrame(random, img3, randTime);
		else if(randomNum == 4)
			addFrame(random, img4, randTime);
		else if(randomNum == 5)
			addFrame(random, img5, randTime);
		else if(randomNum == 6)
			addFrame(random, imgEff1, randTime);
		else if(randomNum == 7)
			addFrame(random, imgEff2, randTime);
		else if(randomNum == 8)
			addFrame(random, imgEff3, randTime);
		else if(randomNum == 9)
			addFrame(random, imgEff4, randTime);
		else
			addFrame(random, imgEff5, randTime);
	}
	
	// Operation 16-save random animation to GIF file
	saveAnimation(random, randomAnim);
	
	// Display GIFs on a Web browser (Windows only)
	showWebPage(webPage);
	 
  	return 0;
}
