#include <swapfile.h>
#include <vnode.h>
#include <pt.h>

/*struct vnode *swapfile;*/

/*void init_swap() {                                                         */
/*  int err = vfs_open("SWAPFILE", O_RDWR | O_CREAT | O_TRUNC, 0, &swapfile);*/

/*  if (err != 0) {                                                          */
/*    return err;                                                            */
/*  }                                                                        */
/*}                                                                          */

/*void write_swap(vaddr_t va, paddr_t pa) {                                                               */
/*  int spl = splhigh();                                                                                  */
/*  struct iovec swap_iov;                                                                                */
/*  struct uio swap_uio;                                                                                  */
/*  uio_kinit(&swap_iov, $swap_uio, (void*)PADDR_TO_KVADDR(pa & PAGE_FRAME), PAGE_SIZE, offset, UIO_READ);*/
/*}                                                                                                       */
