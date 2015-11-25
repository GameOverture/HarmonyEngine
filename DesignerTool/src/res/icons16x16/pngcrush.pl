# Run this script to remove the runtime-warning "libpng warning: iCCP: known incorrect sRGB profile" from QtCreator

my @files = <*.png>;
for $file (@files)
{
	system("pngcrush_1_7_88_w64.exe -ow -rem allb -reduce $file");
}

print "Finished";