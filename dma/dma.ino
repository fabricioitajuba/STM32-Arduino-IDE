#include <dma_private.h>
#include <include\libmaple\i2c.h>

uint8_t data[] = {13,37};
void setup() {
  Serial.begin(115200);
  pinMode(32, INPUT);
  pinMode(33, OUTPUT);
  testDMA();
}


void loop() { 
  delay(500);
  digitalWrite(33, !digitalRead(33));
}

void testDMA()
{
    delay(10);
    setupDMA();//set up the dma before doing anything with the peripheral
    setupI2C();//set up the peripheral
    i2c_peripheral_enable(I2C1);//enable the port
    i2c_start_condition(I2C1);//set the start condition
    uint32_t sr1 = I2C1->regs->SR1;
    uint32_t sr2 = I2C1->regs->SR2;
    while(!(sr1&I2C_SR1_SB))//wait for the start bit to be set
    {
      sr1 = I2C1->regs->SR1;
      sr2 = I2C1->regs->SR2;
    }
    i2c_write(I2C1, 96<<1);//write the address of the device you want to contact (shifted 1 to the left)
    sr1 = I2C1->regs->SR1;
    sr2 = I2C1->regs->SR2;
    while(!(sr1&I2C_SR1_ADDR))//wait for the ACK of the device. If no ACK is received, the I2C_SR1_AF bit will be set instead, this code doesn't check for that
    {
      sr1 = I2C1->regs->SR1;
      sr2 = I2C1->regs->SR2;
    }
    //after the ack a dma request will be sent, and the dma controller will take over
}

void setupI2C()
{
  i2c_stop_condition(I2C1);//always good to start with, if the I2C bus ended in an invalid state, setting the stop condition usually resets it
  i2c_peripheral_disable(I2C1);//and also disable it to be sure
  i2c_init(I2C1);//initialize it
  i2c_config_gpios(I2C1);//configure the gpios
  I2C1->regs->CR2 = I2C_CR2_DMAEN | 36; //dma enabled, peripheral frequency is 36Mhz
  I2C1->regs->CCR = I2C_CCR_FS | 30;
  I2C1->regs->TRISE = 11;
}

void setupDMA()
{
  dma_tube_config i2cConfig =
  {
    data //the memory we want to do DMA from
    ,DMA_SIZE_8BITS //the memory access will be 8 bits
    ,&(I2C1->regs->DR) //the dma will go towards the data register of the i2c port
    ,DMA_SIZE_8BITS //the i2c port is 8 bits
    ,2 // we need to transfer 2 times, so 2 bytes in this case
    ,DMA_MINC_MODE|DMA_CFG_CIRC //we enable memory increment & circular dma. So after the first transfer it goes to the second byte, after the second transfer it starts over again
    ,0
    ,DMA_REQ_SRC_I2C1_TX//the source of the interrupts is the transmit buffer empty interrupt if i2c1
  };
  dma_init(DMA1);//init DMA controller 1
  if(dma_tube_cfg(DMA1, DMA_CH6, &i2cConfig) > 0) //for transmission over I2C1 we need channel 6 (see docs)
  dma_set_priority(DMA1, DMA_CH6, DMA_PRIORITY_VERY_HIGH);
  dma_enable(DMA1, DMA_CH6);
}

//prints the status of the I2C peripheral
void printStatus(char* position)
{
    Serial.println(position);
    Serial.println("CR1");
    Serial.println(I2C1->regs->CR1);
    Serial.println("CR2");
    Serial.println(I2C1->regs->CR2);
    Serial.println("OAR1");
    Serial.println(I2C1->regs->OAR1);
    Serial.println("OAR2");
    Serial.println(I2C1->regs->OAR2);
    Serial.println("DR");
    Serial.println(I2C1->regs->DR);
    Serial.println("SR1");
    Serial.println(I2C1->regs->SR1);
    Serial.println("SR2");
    Serial.println(I2C1->regs->SR2);
    Serial.println("CCR");
    Serial.println(I2C1->regs->CCR);
    Serial.println("TRISE");
    Serial.println(I2C1->regs->TRISE);
}

//prints the status of dma channel 6
void printDmaStatus(char* position)
{
    Serial.println(position);
    Serial.println("CCR6");
    Serial.println(DMA1->regs->CCR6);
    Serial.println("CPAR6");
    Serial.println(DMA1->regs->CPAR6);
    Serial.println("CMAR6");
    Serial.println(DMA1->regs->CMAR6);
    Serial.println("CNDTR6");
    Serial.println(DMA1->regs->CNDTR6);
    Serial.println("IFCR");
    Serial.println(DMA1->regs->IFCR);
    Serial.println("ISR");
    Serial.println(DMA1->regs->ISR);;
}
