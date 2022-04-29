install.packages("readxl")
install.packages("ggpubr")

library("ggplot2")
library("readxl")
library("gtable")
library(ggpubr)

setwd("C:/Users/tao97/OneDrive - purdue.edu/2022_Spring/ABE314/Final_Project/Report/Data")

my_data_1 <- read_excel("Data_collection.xlsx", sheet=1)
my_data_2 <- read_excel("Data_collection.xlsx", sheet=2)
my_data_3 <- read_excel("Data_collection.xlsx", sheet=3)
my_data_4 <- read_excel("Data_collection.xlsx", sheet=4)


tiff("test.tiff", units="in", width=15, height=10, res=300)

plot1 <- ggplot(my_data_1, aes(`Index`, `rotationalSpeed`)) + 
  geom_point() + 
  geom_line() +
  xlim(0,300) +
  ylim(-30,30) +
  theme_light() +
  xlab(bquote("Index")) + 
  ylab("Rotational speed") +
  theme(plot.title = element_text(hjust = 0.5), text = element_text(size = 14), 
        plot.margin = unit(c(0.3, 0.5, 0, 0), "cm")) 

plot2 <- ggplot(my_data_2, aes(`Index`, `rotationalSpeed`)) + 
  geom_point() + 
  geom_line() +
  xlim(0,300) +
  ylim(-30,30) +
  theme_light() +
  theme(plot.title = element_text(hjust = 0.5), text = element_text(size = 14), 
        plot.margin = unit(c(0.3, 0.5, 0, 0), "cm")) 

plot3 <- ggplot(my_data_3, aes(`Index`, `rotationalSpeed`)) + 
  geom_point() + 
  geom_line() +
  xlim(0,300) +
  ylim(-30,30) +
  theme_light() +
  theme(plot.title = element_text(hjust = 0.5), text = element_text(size = 14), 
        plot.margin = unit(c(0.3, 0.5, 0, 0), "cm")) 
plot4 <- ggplot(my_data_4, aes(`Index`, `rotationalSpeed`)) + 
  geom_point() + 
  geom_line() +
  xlim(0,300) +
  ylim(-30,30) +
  theme_light() +
  theme(plot.title = element_text(hjust = 0.5), text = element_text(size = 14), 
        plot.margin = unit(c(0.3, 0.5, 0, 0), "cm")) #+ 
  #scale_y_continuous(breaks = pretty(my_data_4$rotationalSpeed, n = 5)) 

ggarrange(plot1,plot2,plot3,plot4,
          labels = c("Linear:17   Kp=0.01", "Linear:17   Kp=0.02", 
                     "Linear:22   Kp=0.01", "Linear:22   Kp=0.02"),
          ncol = 2, nrow = 2)

dev.off()


