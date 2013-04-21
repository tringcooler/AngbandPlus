;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: LANGBAND -*-

#|

DESC: base.lisp - basic code for the rest of the game
Copyright (c) 2000-2001 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :langband)

(eval-when (:compile-toplevel :load-toplevel :execute)
  ;; 28 bits
  (deftype u-fixnum () '(unsigned-byte 28))
  (deftype vinfo-bit-type () `(unsigned-byte 32))

  (defclass activatable ()
    ((activated :accessor activated? :initform nil))
    (:documentation "Mixin-class for activatation of objects,
may be removed later for efficiency-reasons."))
  
)

(defmacro defsubst (name arglist &body body)
  "Declare an inline defun."
  `(progn
    (declaim (inline ,name))
    (defun ,name ,arglist ,@body)))

(defmacro defcustom (name type init doc)
  "Define a typed variable."
  `(progn
    (declaim (type ,type ,name))
    (defvar ,name (the ,type ,init) ,doc)))

(defmacro defconst (name type init doc)
  "Define a typed constant."
  `(progn
    (declaim (type ,type ,name))
    (defconstant ,name (the ,type ,init) ,doc)))

(defmacro with-type (type expr)
  "Evaluate the arithmetic expression in TYPE.
Adopted from P.Graham `ANSI CL', p 410; with some modifications."
  `(the ,type
    ,(if (and (consp expr)
              (member (car expr) '(+ - * / 1+ 1- random abs sin cos tan cot
                                   signum log exp expt)
                      :test #'eq))
         (let ((nexp
                (labels ((binarize (expr)
                           (if (and (nthcdr 3 expr)
                                    (member (car expr) '(+ - * /)))
                               (destructuring-bind (op a1 a2 . rest) expr
                                 (binarize `(,op (,op ,a1 ,a2) ,@rest)))
                               expr)))
                  (binarize expr))))
           `(,(car nexp) ,@(mapcar #'(lambda (ee) `(with-type ,type ,ee))
                                   (cdr nexp))))
         expr)))


(defmacro while (test &body body)
  "repeat BODY while TEST is true"
  `(do ()
       ((not ,test))
     ,@body))

(defun split-seq-on (str &optional (ch #\Space))
  "returns a list of strings formed by breaking STR at every occurance
of CH (which is not included).  Works for any sequence, not just strings,
but optimized for vectors."
  (when str
    (do* ((prev-pos 0 (1+ next-pos))
          (next-pos (position ch str)
                    (position ch str :start prev-pos))
          (stuff (list (subseq str 0 next-pos))
                 (cons (subseq str prev-pos next-pos)
                       stuff)))
        ((null next-pos) (nreverse stuff)))))



(defgeneric dump-object (obj stream style)
  (:documentation "Dumps an object to a stream in a certain style."))

(defmethod dump-object (obj stream style)
  (declare (ignore stream style))
  (warn "DUMP-OBJECT not implemented for ~a" (type-of obj)))

(defgeneric trigger-event (obj event arg-list)
  (:documentation "Triggers a given event on the object. Recursive."))

(defmethod trigger-event (obj event arg-list)
  (declare (ignore obj event arg-list))
  (values))

(defun apply-event (event event-list arg-list)
  "Iterates through event-list and funcalls any events
with given arg-list if any events match."
  (dolist (i event-list)
    (when (eq event (car i))
      (apply (cdr i) arg-list))))


#||
(defun register-variant& (id name &key before-game-init after-game-init)
  "Registers variant in appropriate places."
  
  (setf (get 'variant 'id) id
	(get 'variant 'name) name
	(get 'variant 'pre-init) before-game-init
	(get 'variant 'post-init) after-game-init))
||#

(defun register-variant-common& (&key before-game-init after-game-init)
  "Registers callbacks for common functions for all variants. Called
before variant init-functions."
  (setf (get 'common 'pre-init) before-game-init
	(get 'common 'post-init) after-game-init))


(defun symbolify (data)
  "Returns a symbol in a form which can be understood when reading code."
  (if (eq data nil)
      nil
      `',data))

(defsubst i2a (num)
  "Returns the letter corresponding to #\a + num."
  (declare (type u-fixnum num))
  (code-char (with-type u-fixnum (+ (char-code #\a) num))))

(defsubst a2i (char)
  "Returns the number corresponding to the char given, where #\a is 0."
  (- (char-code char) (char-code #\a)))
#||
(defsubst randint (num)
  "Returns (1+ (random num))."
  (declare (type u-fixnum num))
  (with-type u-fixnum (1+ (random num))))
||#

;; hack ever so long
(defun randint (num)
  (1+ (random num)))

#||
(defsubst rand-range (a b)
  "Returns a random numer in the range [a..b]."
  (declare (type u-fixnum a b))
  (with-type u-fixnum (+ a (random (1+ (- b a))))))
||#

;; hack ever so long
(defun rand-range (a b)
  (+ a (random (1+ (- b a)))))

#+cmu
(defsubst int-/ (a b)
;;  (declare (type u-fixnum a b))
;;  (the u-fixnum
    (floor a b)
;;    )
  )

#-cmu
(defmacro int-/ (a b)
  "Integer division, as in C."
  `(the fixnum (floor ,a ,b)))


(defun shrink-array! (arr)
  "Shrinks the array and removes NIL gaps. Returns the new size."

  (let ((len (length arr))
	(cur-write 0)
	(cur-obj nil))
    (declare (type u-fixnum cur-write len))
    
    (loop for cur-read of-type fixnum from 0 to (the fixnum (1- len))
	  do
	  (setq cur-obj (aref arr cur-read))
	  (when cur-obj
	    (setf (aref arr cur-write) cur-obj)
	    (incf cur-write)))

    (setf (aref arr cur-write) nil)
    
    cur-write))


(defun add-object-to-array! (arr cur-size max-size aobj)
  "Adds an object to array. Returns T if succesful
and NIL if unsuccesful."
  (declare (type u-fixnum cur-size max-size))
  (cond ((< cur-size max-size)
	 ;; we have room
;;	 (warn "Adding ~a to array" aobj)
	 (setf (aref arr cur-size) aobj)
	 t)
	;; we're full
	(t
	 (warn "equipment full..")
	 nil)))


(defun htbl-to-vector (htbl &key sort-table-p sorted-by-key sorted-by-fun fill-pointer)
  "Takes a hash-table and returns a vector with the elements."
  
  (let* ((len (hash-table-count htbl))
	 (arr (if fill-pointer
		  (make-array len :initial-element nil :fill-pointer t)
		  (make-array len :initial-element nil))))
	 
    (declare (type u-fixnum len))
    
    (loop for i of-type u-fixnum from 0
	  for x being the hash-values of htbl
	  do
	  (setf (aref arr i) x))
    
    (when sort-table-p
      (let ((sort-args (list arr (if sorted-by-fun sorted-by-fun #'<))))
	(when sorted-by-key
	  (setq sort-args (append sort-args (list :key sorted-by-key))))
	(setq arr (apply #'sort sort-args))))
    
    arr))

(defun shuffle-array! (tmp-arr len)
  "Shuffles the given array"
  (declare (type u-fixnum len))
  
  (loop for i of-type u-fixnum from 0 to (1- len)
	for rnd-val = (random len)
	do
	(rotatef (aref tmp-arr i) (aref tmp-arr rnd-val)))
  
  tmp-arr)

(defun get-array-with-numbers (len &key fill-pointer)
  "Returns an array with increasing numbers."
  (let ((arr (if fill-pointer
		 (make-array len :fill-pointer t)
		 (make-array len))))
    
    (loop for i from 0 to (1- len)
	  do
	  (setf (aref arr i) i))
    
    arr))

(defun parse-dice (str)
  "Parses a dice and returns a CONS with num-dice and base-dice."
  (let ((pos (position #\d str)))
    (cons (subseq str 0 pos)
	  (subseq str (1+ pos)))))


(defmacro bit-flag-add! (loc &rest flags)
  "Same as 'loc |= flags', and uses LOGIOR."
  `(setf ,loc (logior ,loc ,@flags)))

(defmacro bit-flag-remove! (loc flag)
  "Same as 'loc &= ~(flag)', and uses LOGANDC2."
  `(setf ,loc (logandc2 ,loc ,flag)))

;; change me into a macro at some point?
#+allegro
(defmacro bit-flag-set? (loc flag)
  `(/= 0 (logand ,loc ,flag)))

#-allegro
(defun bit-flag-set? (loc flag)
  "Checks if the given flag is set, and returns T or NIL."
  (/= 0 (logand loc flag)))

#+allegro
(defmacro bit-flag-and (pos1 pos2)
  `(/= 0 (logand ,pos1 ,pos2)))

#-allegro
(defun bit-flag-and (pos1 pos2)
  (/= 0 (logand pos1 pos2)))

(defsubst read-one-character ()
  "Reads one character from the C-side."

  #+allegro
  (c-inkey)
  #+cmu
  (code-char (c-inkey)))

(defsubst clear-the-screen ()
  "Clears the screen on the C-side."
  (c-term-clear)
  #+cmu
  (c-clear-from 0))

;; move to better place later
(defgeneric print-depth (level setting)
  (:documentation "fix me later.. currently just prints depth."))

;; see print.lisp

;; move me later
(defgeneric activate-object (obj &key &allow-other-keys)
  (:documentation "Most objects in Langband is created lazily.
This means that an object may be created but may not be fully initialised
and filled with appropriate values right away.  The normal CL/CLOS mechanisms
deal with the actual creation of the bare object, but non-trivial objects
should also be \"activated\", ie get proper values on all variables.
The object in question should be returned."))  

(defmethod activate-object (obj &key)

  obj)

(defmethod activate-object :after ((obj activatable) &key)
   (setf (activated? obj) t))


#||
;; move me later
(defgeneric post-initialise (obj &key &allow-other-keys)
  (:documentation "Is called to complete an initialisation of an object.
Should return the object."))

(defmethod post-initialise (obj &key)

  obj)
||#

;; move me later

(defgeneric find-appropriate-monster (level room player)
  (:documentation "Returns an appropriate monster for a given
level/room/player combo.  Allowed to return NIL."))

(defmethod find-appropriate-monster (level room player)
  (declare (ignore room player))
  (error "No proper FIND-APPROPRIATE-MONSTER for ~s" (type-of level)))

(defgeneric initialise-monsters& (variant &key &allow-other-keys)
  (:documentation "Initialises monsters for the given variant."))
  
(defmethod initialise-monsters& (variant &key)
  (error "No INIT-MONSTERS for ~s" (type-of variant)))

(defgeneric initialise-features& (variant &key &allow-other-keys)
  (:documentation "Initialises features for the given variant."))
  
(defmethod initialise-features& (variant &key)
  (error "No INIT-FEATURES for ~s" (type-of variant)))

(defgeneric initialise-objects& (variant &key &allow-other-keys)
  (:documentation "Initialises objects for the given variant."))
  
(defmethod initialise-objects& (variant &key)
  (error "No INIT-OBJECTS for ~s" (type-of variant)))

;; move me later

(defun compile-in-environment (func)
  (let (
	#+cmu (*compile-print* nil)
	      #+cmu (*load-verbose* nil)
	      (*load-print* nil)
	      ;;#+cmu (*error-output* o-str)
	      #+cmu (extensions:*gc-verbose* nil)
	      )
    (funcall func)))

(defun text-to-ascii (str)
  "converts a c-type string to a lisp-string in ascii."
  
  (let ((backslashed nil)
	(controlled nil))
    
    (with-output-to-string (s)
      (loop for x across str
	  do 
	    ;;(warn "checking ~s" x)
	    (cond (backslashed
		   (case x
		     (#\\ (write-char #\\ s))
		     (#\s (write-char #\Space s))
		     (#\b (write-char #\Backspace s))
		     (#\n (write-char #\Linefeed s))
		     (#\r (write-char #\Return s))
		     (#\t (write-char #\Tab s))
		     ;; skip hex
		     (otherwise
		      (write-char x s)))
		   (setq backslashed nil))

		  (controlled
		   (write-char (code-char (logand (char-code x) #o37)) s)
		   (setq controlled nil))
		  ((eql x #\\) 
		   (setq backslashed t))
		  ((eql x #\^)
		   (setq controlled t))
		  
		  (t
		   (write-char x s))))
      s)))

;;(trace text-to-ascii)

#+allegro
(defmacro tricky-profile (expr type)
  `(prof:with-profiling (:type ,type)
    (prog1
	,expr
      (with-open-file (s (pathname "prof.dump")
			    :direction :output
			    :if-exists :supersede)
	   (prof:show-flat-profile :stream s :verbose t)
	   (prof:show-call-graph :stream s :verbose t)
	   ))))

#+cmu
(defmacro tricky-profile (expr type)
  (declare (ignore type))
  `(time ,expr))

#||
(defvar *left-adj-6str* (make-hash-table :test #'eql))

(dotimes (i 60)
  (setf (gethash i *left-adj-6str*) (format nil "~6d" i)))

(defmacro %get-6str (num)
  `(progn
    (assert (< ,num 60)) ;; the length
    (gethash ,num *left-adj-6str*)))
||#

(defun-memo %get-6str (num)
  (format nil "~6d" num))

(defun-memo %get-5str (num)
  (format nil "~5d" num))

(defun-memo %get-4str (num)
  (format nil "~4d" num))

(defun-memo %get-8str (num)
  (format nil "~8d" num))

(defun-memo %get-9str (num)
  (format nil "~9d" num))

(defun-memo %get-13astr (val)
  (format nil "~13@a" val))
