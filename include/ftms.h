#ifndef FTMS_FORMATTER__
#define FTMS_FORMATTER__

#include <cstdlib> // for malloc and free
#include <fstream>
#include <iostream>

// one space reserved for the terminator, so a max of 15 data attributes per pixel.
using namespace std;

const unsigned int MAX_FSTRING_LEN = 16;

struct ftms_info{
	unsigned int image_width;
	unsigned int image_height;
	unsigned int pixel_size;
	unsigned long image_size;
	char fstring[MAX_FSTRING_LEN];
};

// function prototypes
static int ftms_confirm_pixel_size(ftms_info *data); // returns 0 if pixel size matches computed struct length from fstring
static int ftms_confirm_image_size(ftms_info *data); // returns 0 if image size matches computed size from w, h, chunk size
static unsigned int ftms_calc_pixel_size(char *fstring); // returns the number of bytes each pixel should occupy given the fstring, helper for the above

static ftms_info *ftms_read_info(const char *src); // returns info if successfully read data, otherwise NULL
static void ftms_print_info(ftms_info *data); // print image info: see implementation for char -> datatype map
static void ftms_free_info(ftms_info *data); // free allocated memory

static ftms_info *ftms_read(const char *src, char *buf); // main reading function
static int ftms_write(const char *dst, ftms_info* data, char *buf); // main writing function

int ftms_confirm_pixel_size(ftms_info *data){
	return (ftms_calc_pixel_size(data->fstring) == data->pixel_size) ? 0 : -1;
}

int ftms_confirm_image_size(ftms_info *data){
	return (data->pixel_size*data->image_width*data->image_height == data->image_size) ? 0 : -1;
}

/* NOTE: this function doesn't take into account standard cstruct padding
 *
 * CHAR -> TYPE MAP:
 * 	u: unsigned int
 * 	U: unsigned long
 * 	i: int
 * 	I: long int
 * 	c: char
 * 	f: half-float
 * 	F: float
 * 	d: double		 */

unsigned int ftms_calc_pixel_size(char *fstring){
	unsigned int sum = 0;
	for(unsigned int i = 0; i < MAX_FSTRING_LEN-1; i++){ // index 15 would always be a null terminator, as we always ensure one on load, so do 15-1
		if(fstring[i] == '\0'){break;}
		switch (fstring[i]){
			case ('f'): {sum += 2; break;}
			case ('u'):
			case ('i'):
			case ('F'): {sum += 4; break;}
			case ('d'):
			case ('I'):
			case ('U'): {sum += 8; break;}
			default: {
				cout << "tms format: ftms_calc_pixel_size: unknown format character: " << fstring[i] << endl; return 0;
			}
		}
	}
	return sum;
}


ftms_info *ftms_read_info(const char *src){

	ifstream read_stream;
	read_stream.open(src, std::ios::in | std::ios::binary);
	if(read_stream.fail()){ cout << "tms format: ftms_read_info: couldn't open file: " << src << endl; read_stream.close(); return nullptr; }

	unsigned char cap = read_stream.get();
	if(read_stream.fail()){ cout << "tms format: ftms_read_info: couldn't read file: " << src << endl; read_stream.close(); return nullptr; }
	if(cap != 0xFF){ cout << "tms format: ftms_read_info: file missing 0xFF cap byte: " << src << endl; read_stream.close(); return nullptr; }

	ftms_info *data = (ftms_info *) malloc(sizeof(ftms_info));

	read_stream.read(data->fstring, 16); // get automatically appends a null terminator, so we use read

	bool found_null_term = false;
	for(int i = 0; i < 16; i++){
		if(data->fstring[i] == '\0'){found_null_term = true; break;}
	}
	if(!found_null_term){ cout << "tms format: ftms_read_info: format string missing null terminator: " << src << endl; free(data); read_stream.close(); return nullptr; }

	char *buf = (char *) malloc(sizeof(char) * 20); // read into a buf so we can ensure all 20 bytes were written with one check
	read_stream.read(buf, 20);
	if(read_stream.gcount() != 20){ cout << "tms format: ftms_read_info: too few bytes read in header: " << src << endl; read_stream.close(); free(data); free(buf); return nullptr; }

	data->image_width = *((int *)(&buf[0]));
	data->image_height = *((int *)(&buf[4]));
	data->pixel_size = *((int *)(&buf[8]));
	data->image_size = *((long int *)(&buf[12]));

	read_stream.close();
	free(buf);

	return data;
}

void ftms_print_info(ftms_info* data){
	cout << "FILE DATA:";
	cout << "\n\tformat: " << data->fstring << ", given chunk size: " << data->pixel_size;
	cout << ((ftms_confirm_pixel_size(data) == 0) ? ", confirmed " : ", contested ") << "by test function";
	cout << "\n\twidth(cols): " << data->image_width << ", height(rows): " << data->image_height;
	cout << "\n\tgiven image size of " << data->image_size;
	cout << ((ftms_confirm_image_size(data) == 0) ? ", confirmed " : ", contested ") << "by test function" << endl;
}

void ftms_free_info(ftms_info* data){ free(data); }

// temp function
void ftms_test_put_info(ftms_info* data, const char* dst){
	ofstream write_stream;
	write_stream.open(dst, std::ios::out | std::ios::binary);
	if(write_stream.fail()){ cout << "couldn't open " << dst << endl; return; }

	char cap = 0xFF;
	write_stream.put(cap);

	write_stream.write(data->fstring, MAX_FSTRING_LEN);
	write_stream.write((char *)&data->image_width, 4);
	write_stream.write((char *)&data->image_height, 4);
	write_stream.write((char *)&data->pixel_size, 4);
	write_stream.write((char *)&data->image_size, 8);

	write_stream.close();
	return;
}

ftms_info* ftms_read(const char *src, char *buf){

	// SETTING UP PIPE //
	ifstream read_stream;
	read_stream.open(src, std::ios::in | std::ios::binary);
	if(read_stream.fail()){ cout << "tms format: ftms_read_info: couldn't open file: " << src << endl; read_stream.close(); return nullptr; }

	unsigned char cap = read_stream.get();
	if(read_stream.fail()){ cout << "tms format: ftms_read_info: couldn't read file: " << src << endl; read_stream.close(); return nullptr; }
	if(cap != 0xFF){ cout << "tms format: ftms_read_info: file missing 0xFF cap byte: " << src << endl; read_stream.close(); return nullptr; }

	// PARSE HEADER //
	ftms_info *data = (ftms_info *) malloc(sizeof(ftms_info));
	read_stream.read(data->fstring, 16); // get automatically appends a null terminator, so we use read

	bool found_null_term = false;
	for(int i = 0; i < 16; i++){
		if(data->fstring[i] == '\0'){found_null_term = true; break;}
	}
	if(!found_null_term){ cout << "tms format: ftms_read: format string missing null terminator: " << src << endl; read_stream.close(); free(data); return nullptr; }

	char *info_buf = (char *) malloc(sizeof(char) * 20); // read into a buf so we can ensure all 20 bytes were written with one check
	read_stream.read(info_buf, 20);
	if(read_stream.gcount() != 20){ cout << "tms format: ftms_read: too few bytes read in header: " << src << endl; read_stream.close(); free(data); return nullptr; }

	data->image_width = *((int *)(&info_buf[0]));
	data->image_height = *((int *)(&info_buf[4]));
	data->pixel_size = *((int *)(&info_buf[8]));
	data->image_size = *((long int *)(&info_buf[12]));

	free(info_buf);

	// check that data info makes sense
	if(!(ftms_confirm_pixel_size(data) == 0) || !(ftms_confirm_image_size(data) == 0)){
		cout << "tms format: ftms_read: bad header data, use ftms_print_data for more specific info" << endl;
		read_stream.close();
		free(data);
		return nullptr;
	}

	// READ DATA INTO BUF //
	unsigned char pixel_cap = read_stream.get();
	if(pixel_cap != 0xFF){ cout << "tms format: ftms_read_info: file missing 0xFF pixel_cap byte: " << src << endl; free(data); read_stream.close(); return nullptr; }

	unsigned int readbytes = 0;
	unsigned int step = 1024;
	for(; readbytes < data->image_size; readbytes += step){

		while(data->image_size - (readbytes + step) < 0){ step /= 2; } // divide our block size to accommodate nearing eof

		read_stream.read(&buf[readbytes], step);
		if(read_stream.gcount() != step){
			cout << "tms format: ftms_read: premature EOF - expected " << (data->image_size - readbytes) << " more bytes" << endl;
			read_stream.close();
			free(data);
			return nullptr;
		}
	}

	unsigned char end_cap = read_stream.get();
	if(end_cap != 0xFF){ cout << "tms format: ftms_read_info: file missing 0xFF end_cap byte: " << src << endl; read_stream.close(); free(data); return nullptr; }
	read_stream.close();

	return data;
}

int ftms_write(const char *dst, ftms_info* data, char *buf){

	ofstream write_stream;
	write_stream.open(dst, std::ios::out | std::ios::binary);
	if(write_stream.fail()){ cout << "tms format: ftms_write: couldn't open " << dst << endl; write_stream.close(); return -1; }

	char cap = 0xFF;
	write_stream.put(cap);
	if(write_stream.fail()){ cout << "tms format: ftms_write: write fail at " << dst << endl; write_stream.close(); return -1; }

	write_stream.write(data->fstring, MAX_FSTRING_LEN);
	write_stream.write((char *)&data->image_width, 4);
	write_stream.write((char *)&data->image_height, 4);
	write_stream.write((char *)&data->pixel_size, 4);
	write_stream.write((char *)&data->image_size, 8);

	write_stream.put(cap);

	unsigned int writtenbytes = 0;
	unsigned int step = 1024;
	for(; writtenbytes < data->image_size; writtenbytes += step){

		while(data->image_size - (writtenbytes + step) < 0){ step /= 2; } // divide our block size to accommodate nearing eof

		write_stream.write(&buf[writtenbytes], step);
		if(write_stream.fail()){
			cout << "tms format: ftms_write: unknown writing error - wrote " << writtenbytes << "/" << data->image_size << "before failing" << endl;
			write_stream.close();
			return -1;
		}
	}

	write_stream.put(cap);

	write_stream.close();

	return 0;
}

#endif
