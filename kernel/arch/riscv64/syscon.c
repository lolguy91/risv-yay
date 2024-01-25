
unsigned int *syscon = (unsigned int *)0x100000; 

void reboot() {
    *syscon = 0x7777;
    return;
}
void poweroff() {
    *syscon = 0x5555;
    return;
}