//
//  Logger.h
//  MyKext
//
//  Created by Daniel Lifshitz on 16/07/2025.
//

#define DRIVER_ID "MyKext:"

#define LOG(logLevel, message, ...) printf(DRIVER_ID " level: %d, file: %s, function: %s, line %d " message "\n" , logLevel, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
