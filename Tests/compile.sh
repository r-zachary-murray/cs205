#Compile frames. 

for i in $(ls -d */);
#	do ls ${i}"frames";
	do ffmpeg -framerate 15 -i ${i}"frames/"%d.png -qscale:v 0 ${i}"/out.avi"; ffmpeg -i ${i}"/out.avi" -vf scale=1000:1000 ${i}"/out.gif"; rm -r ${i}"frames/";
done
