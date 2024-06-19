#include "__CHECKSUM__.h"

using namespace std;

double send_value[5] = {571229,1.157314,4.421715,1.490760,0.321791};
double rec_value[5] = {571229,1.157314,4.421715      ,1.490760,0.321791};
double cor_value[5] = {571229,1.157314,4.421715000006,1.490760,0.321791};

int main(){
	uint32_t send_crc = vector_checksum_crc(send_value, 5);
	cout << send_crc << '\n';

	uint32_t rec_crc = vector_checksum_crc(rec_value, 5);
	cout << rec_crc << '\n';

	uint32_t cor_crc = vector_checksum_crc(cor_value, 5);
	cout << cor_crc << '\n';



}
