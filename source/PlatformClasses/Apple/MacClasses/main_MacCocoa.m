
int main(int argc, const char *argv[]) 
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    int ret = NSApplicationMain(argc, argv);
	[pool release];
	return ret;
}
