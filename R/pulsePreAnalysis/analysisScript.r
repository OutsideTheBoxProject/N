# reading in data input
log00 = read.table("./data/LOGGER00.TXT")
log01 = read.table("./data/LOGGER01.TXT")
log02 = read.table("./data/LOGGER02.TXT")
log03 = read.table("./data/LOGGER03.TXT")
log04 = read.table("./data/LOGGER04.TXT")
log05 = read.table("./data/LOGGER05.TXT")
log06 = read.table("./data/LOGGER06.TXT")
log07 = read.table("./data/LOGGER07.TXT")
log08 = read.table("./data/LOGGER08.TXT")
log09 = read.table("./data/LOGGER09.TXT")
log10 = read.table("./data/LOGGER10.TXT")
log11 = read.table("./data/LOGGER11.TXT")
log12 = read.table("./data/LOGGER12.TXT")

# inspect lengths
length(log00$V1)
length(log01$V1)
length(log02$V1)
length(log03$V1)
length(log04$V1)
length(log05$V1)
length(log06$V1)
length(log07$V1)
length(log08$V1)
length(log09$V1)
length(log10$V1)
length(log11$V1)
length(log12$V1)

# count 0oes
sum(log00$V1 == 0)
sum(log01$V1 == 0)
sum(log02$V1 == 0)
sum(log03$V1 == 0)
sum(log04$V1 == 0)
sum(log05$V1 == 0)
sum(log06$V1 == 0)
sum(log07$V1 == 0)
sum(log08$V1 == 0)
sum(log09$V1 == 0)
sum(log10$V1 == 0)
sum(log11$V1 == 0)
sum(log12$V1 == 0)

# remove 0oes
log00_clear = log00$V1
#[!log00$V1 %in% c(0)]
log01_clear = log01$V1
#[!log01$V1 %in% c(0)]
log02_clear = log02$V1
#[!log02$V1 %in% c(0)]
log03_clear = log03$V1
#[!log03$V1 %in% c(0)]
log04_clear = log04$V1
#[!log04$V1 %in% c(0)]
log05_clear = log05$V1
#[!log05$V1 %in% c(0)]
log06_clear = log06$V1
#[!log06$V1 %in% c(0)]
log07_clear = log07$V1
#[!log07$V1 %in% c(0)]
log08_clear = log08$V1
#[!log08$V1 %in% c(0)]
log09_clear = log09$V1
#[!log09$V1 %in% c(0)]
log10_clear = log10$V1
#[!log10$V1 %in% c(0)]
log11_clear = log11$V1
#[!log11$V1 %in% c(0)]
log12_clear = log12$V1
#[!log12$V1 %in% c(0)]

# inspect lengths
length(log00_clear)
length(log01_clear)
length(log02_clear)
length(log03_clear)
length(log04_clear)
length(log05_clear)
length(log06_clear)
length(log07_clear)
length(log08_clear)
length(log09_clear)
length(log10_clear)
length(log11_clear)
length(log12_clear)

# plotting things
png("./plots/log00.png")
plot(log00_clear, type="l", col="red")
dev.off()
png("./plots/log01.png")
plot(log01_clear, type="l", col="red")
dev.off()
png("./plots/log02.png")
plot(log02_clear, type="l", col="red")
dev.off()
png("./plots/log03.png")
plot(log03_clear, type="l", col="red")
dev.off()
png("./plots/log04.png")
plot(log04_clear, type="l", col="red")
dev.off()
png("./plots/log05.png")
plot(log05_clear, type="l", col="red")
dev.off()
png("./plots/log06.png")
plot(log06_clear, type="l", col="red")
dev.off()
png("./plots/log07.png")
plot(log07_clear, type="l", col="red")
dev.off()
png("./plots/log08.png")
plot(log08_clear, type="l", col="red")
dev.off()
png("./plots/log09.png")
plot(log09_clear, type="l", col="red")
dev.off()
png("./plots/log10.png")
plot(log10_clear, type="l", col="red")
dev.off()
png("./plots/log11.png")
plot(log11_clear, type="l", col="red")
dev.off()
png("./plots/log12.png")
plot(log12_clear, type="l", col="red")
dev.off()