/*
 * ipacket.h
 *
 *  Created on: Dec 27, 2020
 *      Author: n00b
 */

#pragma once

#ifndef SOURCE_IPACKET_H_
#define SOURCE_IPACKET_H_

    /*
     * Packets shape:
     * 
     * DEFAULT IPACKET:
     *      [                h   e   a   d   e   r                ][  body   ]
     *      [ PREAMBLE ][ COMMAND ][ SOURCE ][ DESTINATION ][ PID ][ PAYLOAD ]
     *           4           2      LEN_NAME     LEN_NAME      4        #
     * 
     * TCP PACKET:
     *      [           h   e   a   d   e   r         ][       body      ]
     *      [ PREAMBLE ][ SIZE ][ INITIALIZING VECTOR ][ DEFAULT IPACKET ]
     *           4          4      LEN_AES_MASTERKEY         58 + #
     * 
     */

    #include "../common/utils.h"
    #include "packetmap.h"

//     #if defined(ESP8266)
//         #if defined(dump)
//             #undef dump
//         #endif
//     #endif

namespace packet
{
	struct pflags_fst_
	{
        // hashable
        // is_devconf
	};

    struct variable_fst_;  /* [ VariableMap ] */
	struct packet_fst_;    /* [  PacketMap  ] */
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Generic ipacket: base implementation                        |
//      |___________________________________________________________________|
//

    class ipacket
    {
    private:
        iCom_t          _flash_index=0;
        iPid_t          _pid=0;
    protected:
        char        *   _payload=nullptr;
        
    public:
                 ipacket    ();
                 ipacket    (const iCom_t);
                 ipacket    (const ipacket &);
                 ipacket    (ipacket *);
                 ipacket    (char * b, bool wbody);
        virtual ~ipacket    ();   

    public:
        iPid_t          id              (void) { return _pid; }
        bool            init            (iCom_t);
        void            clear           (void);
        bool            parse           (char *b, bool wbody);
        char        *   label           (void) const;
        void            header          (char *b, const char*s, const char *t);
        void            header          (char *b) { header(b, P2C(BOARD_NAME), P2C(DEFAULT_TARGET)); }
        void            forge           (char *b, const char*s, const char *t);
        void            forge           (char *b) { forge(b, P2C(BOARD_NAME), P2C(DEFAULT_TARGET)); };
        char        *   payload         (void) const { return _payload; };
        iCom_t          command         (void) const;
        char        *   description     (void) const;
        iSize_t         data_size       (void) const;
        iSize_t         full_size       (void) const;
        void            checksum        (uint8_t *b);
        void            dump            (void);
        void            eval            (void);

    public:
        
        friend class netpacket;

#if defined SOURCE_IPACKET_H_
		char       *    p_message       (void);
		char       *    p_command       (void);
		char       *    p_name          (void);
		char       *    p_type          (void);
		char       *    p_board         (void);
		uint8_t    *    p_devnum        (void);
		char       *    p_devname       (void);
		char       *    p_newname       (void);
		char       *    p_desc1         (void);
		char       *    p_desc2         (void);
		char       *    p_desc3         (void);
		char       *    p_desc4         (void);
		uint32_t   *    p_epoch         (void);
		uint32_t   *    p_freemem       (void);
		uint32_t   *    p_looptime      (void);
		int32_t    *    p_errors1       (void);
		int32_t    *    p_errors2       (void);
		int32_t    *    p_errors3       (void);
		int32_t    *    p_errors4       (void);
		uint32_t   *    p_totalrx       (void);
		uint32_t   *    p_totaltx       (void);
		int32_t    *    p_value1        (void);
		int32_t    *    p_value2        (void);
		int32_t    *    p_value3        (void);
		int32_t    *    p_value4        (void);
		int32_t    *    p_value5        (void);
		int32_t    *    p_value6        (void);
		int32_t    *    p_value7        (void);
		int32_t    *    p_value8        (void);
		int32_t    *    p_param1        (void);
		int32_t    *    p_param2        (void);
		int32_t    *    p_param3        (void);
		int32_t    *    p_param4        (void);
		int32_t    *    p_param5        (void);
		int32_t    *    p_param6        (void);
		int32_t    *    p_param7        (void);
		int32_t    *    p_param8        (void);
		uint32_t   *    p_stepday1      (void);
		uint32_t   *    p_stepday2      (void);
		uint32_t   *    p_stepday3      (void);
		uint32_t   *    p_stepday4      (void);
		uint32_t   *    p_stephour1     (void);
		uint32_t   *    p_stephour2     (void);
		uint32_t   *    p_stephour3     (void);
		uint32_t   *    p_stephour4     (void);
		uint32_t   *    p_timeout1      (void);
		uint32_t   *    p_timeout2      (void);
		uint32_t   *    p_timeout3      (void);
		uint32_t   *    p_timeout4      (void);
		uint8_t    *    p_level         (void);
		uint8_t    *    p_status        (void);
		uint8_t    *    p_count1        (void);
		uint8_t    *    p_count2        (void);
		uint8_t    *    p_count3        (void);
		uint8_t    *    p_count4        (void);
		uint8_t    *    p_count5        (void);
		uint8_t    *    p_count6        (void);
		uint8_t    *    p_count7        (void);
		uint8_t    *    p_count8        (void);
		uint8_t    *    p_pin1          (void);
		uint8_t    *    p_pin2          (void);
		uint8_t    *    p_pin3          (void);
		uint8_t    *    p_pin4          (void);
		uint32_t   *    p_tcprx         (void);
		uint32_t   *    p_tcptx         (void);
		uint32_t   *    p_serialrx      (void);
		uint32_t   *    p_serialtx      (void);
		char       *    p_ip1           (void);
		char       *    p_ip2           (void);
		char       *    p_ip3           (void);
		char       *    p_ip4           (void);
		uint32_t   *    p_port1         (void);
		uint32_t   *    p_port2         (void);
		uint32_t   *    p_port3         (void);
		uint32_t   *    p_port4         (void);

#if defined(INDOORINO_NETWORK)

		char       *    p_hash1         (void);
		char       *    p_hash2         (void);
		char       *    p_hash3         (void);
		char       *    p_hash4         (void);
		char       *    p_aeskey1       (void);
		char       *    p_aeskey2       (void);

#endif // INDOORINO_NETWORK 

#if defined (__linux__)

		char       *    p_table1        (void);
		char       *    p_table2        (void);
		char       *    p_table3        (void);
		char       *    p_table4        (void);

#endif // __linux__
        
#endif /* This simply make te code collapsable in Kate */
    
    };
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Full ipacket: has source and destination buffer             |
//      |___________________________________________________________________|
//
    
    class netpacket : public ipacket
    {
    public:

        char                    source[LEN_NAME] {0};
        char                    target[LEN_NAME] {0};
        
        netpacket():ipacket() {};
        netpacket(const iCom_t c):ipacket(c) {};
        netpacket(ipacket * p):ipacket(p) {};
        
        netpacket(char * header, bool wbody);
        netpacket(const netpacket &);
        netpacket(netpacket *);
        netpacket(ipacket *, const char *, const char *);
        

        virtual ~netpacket() {};

        void    header          (char*);
        void    forge           (char*);
        
        #if defined(INDOORINO_CLIENT)
        
        void    fromDictionary  (PyObject  *);
        void    toDictionary    (PyObject  *);
        
        #endif /* INDOORINO_CLIENT */

        
    };

//      _____________________________________________________________________
//      |                                                                   |
//      |       Scheduled ipacket: has datetieme                            |
//      |___________________________________________________________________|
//

    class schedPacket : public netpacket
    {
    public:
        schedPacket(netpacket * p);
        ~schedPacket() {};
//         schedPacket():netpacket() {}
//         schedPacket(char * h):netpacket(h) {}
//         schedPacket(ipacket * p, const char * s, const char * t):netpacket(p,s,t) {}
        
        time_t  sendtime=0;
        bool    broadcast=false;
        bool    shell=false;
    };
    

//      _____________________________________________________________________
//      |                                                                   |
//      |       Transient ipacket: has timer and counter                    |
//      |___________________________________________________________________|
//

#if defined(__linux__)
    class bufpacket : public netpacket 
    {
    public:
         bufpacket(netpacket * p):netpacket(p)  {};
        ~bufpacket()                            { data.clear(); }
        std::vector<char>   data;
    };
#endif /* __linux__ */

//      _____________________________________________________________________
//      |                                                                   |
//      |       Packet utils                                                |
//      |___________________________________________________________________|
//

//     void        evaluate            (netpacket * p);

    int16_t     packetsize          (iCom_t);
    int16_t     payloadsize         (iCom_t);
    bool        is_boardconf        (iCom_t);
    bool        is_boardstat        (iCom_t);
    bool        is_devconf          (iCom_t);
    bool        is_devstat          (iCom_t);
    
//      _____________________________________________________________________
//      |                                                                   |
//      |       Practical fuctions to generate packets                      |
//      |___________________________________________________________________|
//

    ipacket *   report              (uint8_t, const char *, const char *);
    ipacket *   epoch               (void);

} /* packet*/
  
    
    
    
    
#endif /* SOURCE_IPACKET_H_ */



