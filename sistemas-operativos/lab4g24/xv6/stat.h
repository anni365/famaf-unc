#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device

struct stat {
  short type;  			// Type of file
  int dev;     			// File system's disk device
  uint ino;    			// Inode number
  short nlink :14; 			// Number of links to file
  uint size;   			// Size of file in bytes
	ushort permissions:2; // Permissions to read and/or write
};
