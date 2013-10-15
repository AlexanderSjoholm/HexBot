void ADCinit()
{
	ADCSRA |= (1<<ADEN)|(1<<ADIE)|(1<<ADPS1)|(1<<ADPS0); // set adc-prescaler, set adc enable and adc interrupt enable
	ADMUX |= (1<<REFS0) | (1 << ADLAR); // set adv vcc ref
	ADMUX |= adc_ch;
}

//läser av alla ir-sensorer och sparar ner dessa
void read_sensors()
{
	mode = read_mode;
	adc_ch = 0;
	ADMUX &= 0xE0;
	ADCSRA |= (1 << ADSC);

}
