from PIL import Image

deltax = 69.3
deltay = 108

currentx = 0
currenty = 0

img = Image.open("allcards.png")

for i in range(4):
	for j in range(13):
		area = (currentx, currenty, currentx+deltax, currenty+deltay)
		cropped_img = img.crop(area)
		title=str(i*13+j+1)+".png"
		cropped_img.save(title)
		currentx += deltax
	currentx = 0
	currenty += deltay

