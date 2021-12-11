      program bridge

      parameter(maxmodes=5000)
      implicit real*8 (a-h,o-z)
      character*3 labs(maxmodes)
      dimension freq(maxmodes),aint(maxmodes),anew(maxmodes)
      character*132 card
      read(5,*)natoms
      nmode=(natoms-2)*3
      lp=(nmode-1)/3+1
      now=0
      do 1 i=1,lp
      read(5,'(a132)')card
      read(card,*)(labs(now+j),j=1,3)
      read(5,'(a132)')card
      read(card(17:132),*)(freq(now+j),j=1,3)
      read(5,'(a132)')card
      read(card(17:132),*)(aint(now+j),j=1,3)
      now=now+3
1     continue
      do 2 i=1,nmode
      write(6,3)i,labs(i),freq(i),aint(i)
3      format(i4,a3,2f10.3)
2      continue
      energy=6.0d00
      ec=energy*1.60217653d-12
      call fortran_c_wrapper(freq,aint,ec,anew,nmode,T)
      write(6,10)T
10     format(' temperature is ',f10.3)
      do 12 i=1,nmode
      write(6,15)i,anew(i)
15     format(i5,e15.5)
12     continue
      stop
      end
