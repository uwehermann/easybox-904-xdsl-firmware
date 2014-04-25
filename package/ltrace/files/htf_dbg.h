#ifndef HTF_DBG_H
#define HTF_DBG_H

#define __HTF_SHOW_PC2(__HTF_PARAM1__)                                      \
                { register unsigned int rpc;                                \
                   asm volatile ( "\n"                                      \
                        "jal __HTF_NEXT_ADDR_" #__HTF_PARAM1__ "\n"         \
                    "__HTF_NEXT_ADDR_" #__HTF_PARAM1__ ":\n"                \
                        "add    %0, $31, $0 \n"                             \
                        : : "r" (rpc) );                                    \
                   printf("src:%d, pc:%08x\n", __HTF_PARAM1__, rpc);        \
                }

// usage:   __HTF_SHOW_PC(__LINE__);
#define __HTF_SHOW_PC(__HTF_PARAM1__)   __HTF_SHOW_PC2(__HTF_PARAM1__)



#define __HTF_ADD_LEBEL2(__HTF_PARAM1__)                       \
                {                                              \
                   asm volatile ( "\n"                         \
                    "__HTF_LABEL_" #__HTF_PARAM1__ ":\n"          \
                        : :  );                                \
                }

// usage:   __HTF_ADD_LEBEL(__LINE__);
#define __HTF_ADD_LEBEL(__HTF_PARAM1__)   __HTF_ADD_LEBEL2(__HTF_PARAM1__)



#endif
