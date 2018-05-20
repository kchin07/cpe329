double true_rms(uint32_t table[], int interval){
    int i = 0;
    double sum = 0;
    int temp;
    for(i = 10; i <= interval+10; i++){
        temp = table[i];
//        print_int32(temp);
//        write_to_terminal(' ');
        sum += temp*temp;
    }
    sum = sum/(interval);
    sum = sqrt(sum);

    return sum;
}

int convert_to_freq(char state, int interval){
    double inverseInterval = 1.0/interval;
    int frequency;
    if(state == 0){
        frequency = (7551.7 * inverseInterval) + 0.0019;
    }
    else{
        frequency = (197988 *inverseInterval) + 0.0624;
    }
    return frequency;
}
