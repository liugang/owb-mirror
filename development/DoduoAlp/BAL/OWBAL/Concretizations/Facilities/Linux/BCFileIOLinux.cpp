/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.h"
#include "BCFileIOLinux.h"

#include DEEPSEE_INCLUDE

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "CString.h"


namespace OWBAL {

File::File(const String path)
    : m_fd(0)
    , m_fileDescriptor(0)
    , m_filePath(path)
    , m_lineNumber(0)
{
}

File::~File()
{
    if (m_fileDescriptor)
        fclose(m_fileDescriptor);
    close();
}

int File::open(char openType)
{
    if (openType == 'w')
        m_fd = ::open(m_filePath.utf8().data(), O_WRONLY|O_CREAT|O_TRUNC, 0666);
    else if (openType == 'r')
        m_fd = ::open(m_filePath.utf8().data(), O_RDONLY);

    return m_fd;
}

void File::close()
{
    if (m_fd >= 0) {
        ::fsync(m_fd);
        ::close(m_fd);
    }
    m_fd = -1;
}

char* File::read(size_t size)
{
    DS_ASS(m_fd, >=, 0);
    char* readData = new char[size + 1];
    ::read(m_fd, readData, size);
    readData[size] = '\0';
    return readData;
}

bool File::readLine(String &line, bool longLine)
{
    if (m_lineNumber == 0) {
        m_fileDescriptor = fdopen(m_fd, "r");
        if (!m_fileDescriptor)
            return false;
    }
    if (longLine)
        return readLongLine(line);
    else
        return readShortLine(line);
}

bool File::readLongLine(String &line)
{
    int character[1] = {0};
    line = String();
    while ((character[0] = fgetc(m_fileDescriptor)) != (int) '\n') {
        if (character[0] == EOF) {
            fclose(m_fileDescriptor);
            m_fileDescriptor = 0;
            m_lineNumber = 0;
            return false;
        }
        line += (char*) character;
    }
    m_lineNumber++;
    return true;
}

bool File::readShortLine(String &line)
{
    const uint8_t maxLength = 80;
    char *lineBuffer = (char *) malloc(maxLength);
    if (fgets(lineBuffer, maxLength, m_fileDescriptor) != NULL) {
        m_lineNumber++;
        line = lineBuffer;
        free(lineBuffer);
        line.remove(line.reverseFind('\n'));
        return true;
    }
    fclose(m_fileDescriptor);
    m_fileDescriptor = 0;
    m_lineNumber = 0;
    return false;
}

void File::write(String dataToWrite)
{
    DS_ASS(m_fd, >=, 0);
    ::write(m_fd, dataToWrite.utf8().data(), dataToWrite.length());
}

int File::getSize()
{
    DS_ASS(m_fd, >=, 0);
    int fileSize, current;

    //save the current offset
    current = lseek(m_fd, 0, SEEK_CUR);

    //save the size
    fileSize = lseek(m_fd, 0, SEEK_END);

    //go back to the previous offset
    lseek(m_fd, current, SEEK_SET);

    return fileSize;
}

}
