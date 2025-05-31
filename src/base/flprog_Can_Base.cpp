#include "flprog_Can_Base.h"

bool FLProgAbstractCanBus::write(CAN_message_t &CAN_tx_msg, bool sendMB)
{
    (void)CAN_tx_msg;
    (void)sendMB;
    return false;
}

//---------------------------------------------------------FLProgCanBusMessage----------------------------------------------------------
FLProgCanBusMessage::FLProgCanBusMessage(FLProgAbstractCanBus *cunBus, uint32_t id, bool extended, uint8_t len)
{
    _message.id = id;
    _message.flags.extended = extended;
    _message.len = len;
    _cunBus = cunBus;
}

void FLProgCanBusMessage::send(bool value)
{
    if (value)
    {
        if (!_sendOldValue)
        {
            _isNeedSend = true;
        }
    }
    _sendOldValue = value;
}

uint8_t FLProgCanBusMessage::getData(uint8_t index)
{
    if (index > 7)
    {
        return 0;
    }
    return _message.buf[index];
}

void FLProgCanBusMessage::setData(uint8_t index, uint8_t value)
{
    if (index > 7)
    {
        return;
    }
    if (_message.buf[index] == value)
    {
        return;
    }
    _message.buf[index] = value;
    if (_mode == FLPROG_CAN_BUS_MESSAGE_PERIODICALLY_MODE)
    {
        return;
    }
    _isNeedSend = true;
}

void FLProgCanBusMessage::pool()
{
    if ((_mode == FLPROG_CAN_BUS_MESSAGE_READ_MODE) || (_mode == FLPROG_CAN_BUS_MESSAGE_SPY_MODE))
    {
        readPool();
        return;
    }
    sendPool();
}

void FLProgCanBusMessage::readPool()
{
    _hasNewData = false;
    if (!_cunBus->hasNewReadMessage())
    {
        return;
    }
    CAN_message_t *_readMessage = _cunBus->getReadMessage();
    if (_mode == FLPROG_CAN_BUS_MESSAGE_SPY_MODE)
    {
        _message.flags.extended = _readMessage->flags.extended;
        _message.id = _readMessage->id;
    }
    else
    {
        if (_readMessage->flags.extended != _message.flags.extended)
        {
            return;
        }
        if (_readMessage->id != _message.id)
        {
            return;
        }
    }
    _hasNewData = true;
    _message.timestamp = _readMessage->timestamp;
    _message.idhit = _readMessage->idhit;
    _message.flags.overrun = _readMessage->flags.overrun;
    _message.flags.reserved = _readMessage->flags.reserved;
    _message.len = _readMessage->len;
    _message.buf[0] = _readMessage->buf[0];
    _message.buf[1] = _readMessage->buf[1];
    _message.buf[2] = _readMessage->buf[2];
    _message.buf[3] = _readMessage->buf[3];
    _message.buf[4] = _readMessage->buf[4];
    _message.buf[5] = _readMessage->buf[5];
    _message.buf[6] = _readMessage->buf[6];
    _message.buf[7] = _readMessage->buf[7];
    _message.mb = _readMessage->mb;
    _message.bus = _readMessage->bus;
    _message.seq = _readMessage->seq;
}

void FLProgCanBusMessage::sendPool()
{
    checkNeedSend();
    if (!_isNeedSend)
    {
        return;
    }
    _cunBus->write(_message);
    _sendTime = millis();
    _isNeedSend = false;
}

void FLProgCanBusMessage::checkNeedSend()
{

    if (_mode == FLPROG_CAN_BUS_MESSAGE_CHANGE_MODE)
    {
        return;
    }
    if (_isNeedSend)
    {
        return;
    }
    if (flprog::isTimer(_sendTime, _sendPeriod))
    {
        _isNeedSend = true;
    }
}

void FLProgCanBusMessage::setMode(uint8_t mode)
{
    if (mode > 4)
    {
        return;
    }
    _mode = mode;
}

void FLProgCanBusMessage::len(uint8_t value)
{
    if (value < 1)
    {
        return;
    }
    if (value > 8)
    {
        return;
    }
    _message.len = value;
}