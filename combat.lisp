;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: LANGBAND -*-

#|

DESC: combat.lisp - the combat-system
Copyright (c) 2000 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :langband)

(defun check-for-hits ()

  )

(defmethod kill-target! (dun x y target)
  "Tries to remove the monster at the location."
  (setf (creature-alive? target) nil)

  (when (typep target 'active-monster)
    (setf (dungeon.monsters dun) (delete target (dungeon.monsters dun))
	  (cave-monsters dun x y) nil))
  nil)

(defmethod cmb-describe-miss (attacker target)

  (c-print-message! (format nil "~a misses the ~a."
			    (get-creature-name attacker)
			    (get-creature-name target)))
  nil)

(defgeneric cmb-hit-creature? (attacker target the-attack)
  (:documentation "will the attacker hit the target?"))

(defmethod cmb-hit-creature? (attk target the-attack)
  (declare (ignore attk target the-attack))
  (error "not impl."))

(defun %calc-perchance (chance the-ac)
  (let ((mid-res (int-/ (* 90 (- chance (int-/ (* 3 the-ac) 4))) chance)))
;;    (warn "mid-res is ~a" mid-res)
    (+ 5 mid-res)))

;;(trace %calc-perchance)

(defun %did-i-hit? (the-target combat-skill the-ac visible-p)
  "Helper-function that checks if something was a hit."
  (declare (ignore the-target))
  
  (let ((k (random 100)))
    ;; instant miss or hit (5% chance each)
    (when (< k 10) (return-from %did-i-hit? (< k 5))))
  
  (unless visible-p
    (setq combat-skill (int-/ combat-skill 2)))

  #+chance-warning
  (warn "Chance to hit '~a' with skill ~s vs AC ~a is ~a%"
	(get-creature-name the-target) combat-skill the-ac
	(%calc-perchance combat-skill the-ac))
  
  ;; this check is not 100% correct, but it does the job fairly accurately
  (if (and (plusp combat-skill)
	   (>= (random combat-skill)
	       (int-/ (* 3 the-ac)
		      4)))
      t
      nil))

;;(trace %did-i-hit?)
		    
(defmethod cmb-hit-creature? ((attacker player) (target active-monster) the-attack)
  (declare (ignore the-attack))
 
    (let* ((bonus 0) ;; (* (+ to-hit for weapon and dex/str) multiplier)
	   (skills (player.skills attacker))
	   (chance (+ (skills.fighting skills) bonus))
	   (monster-ac (get-creature-ac target))
	   (visible-p t))
      
    (%did-i-hit? target chance monster-ac visible-p)))


(defmethod cmb-hit-creature? ((attacker active-monster) (target player) the-attack)
  (declare (ignore the-attack))
    
  (let* ((power 60) ;; hit_hurt
	 (mlvl (monster.level (amon.kind attacker)))
	 (rlev (if (plusp mlvl) mlvl 1)))
    
    (%did-i-hit? target (+ power (* 3 rlev))
		 (get-creature-ac target)
		 t)))


(defmethod cmb-inflict-damage! (attacker target the-attack)
  (declare (ignore the-attack))
  (error "Unknown combo ~s ~s" attacker target))

  
(defun deduct-hp! (target amount)
  (decf (current-hp target) amount))

(defmethod cmb-inflict-damage! ((attacker active-monster) target the-attack)
  
  (let (;;(kind (amon.kind attacker))
	(dmg-dice (attack.damage the-attack)))

    (cond ((not (consp dmg-dice))
	   0)

	  (t
	   (let ((dmg (roll-dice (car dmg-dice) (cdr dmg-dice))))
;;	     (warn "~ad~a gave ~a dmg to attacker (~a -> ~a hps)"
;;		   (car dmg-dice) (cdr dmg-dice) dmg
;;		   (current-hp target) (- (current-hp target) dmg))
	     (deduct-hp! target dmg)
	     dmg))
	  )))


  
(defmethod cmb-inflict-damage! ((attacker player) target the-attack)
  (declare (ignore the-attack))
  (let* ((weapon (get-weapon attacker)))

    (cond ((not weapon)
	   (deduct-hp! target 1)
	   1)
	  (t
	   ;; we have a weapon..
	   (let ((gval (object.game-values weapon)))
	     (assert gval)
	     (let ((dmg (roll-dice (gval.num-dice gval) (gval.base-dice gval))))
;;	       (warn "~ad~a gave ~a dmg to attacker (~a -> ~a hps)"
;;		     (gval.num-dice gval) (gval.base-dice gval) dmg
;;		     (current-hp target) (- (current-hp target) dmg))
	       (deduct-hp! target dmg)
	       dmg))))

    ))

(defun get-attk-desc (the-attack)
  (let* ((descs (variant.attk-descs *variant*))
	 (desc (gethash (attack.kind the-attack) descs)))
    (if desc
	desc
	"hits you.")))

(defun add-attk-desc (var-obj key desc)
  (setf (gethash key (variant.attk-descs var-obj)) desc))

(defmethod cmb-describe-hit ((attacker active-monster) (target player) the-attack)
  (let ((desc (get-attk-desc the-attack)))
    (c-print-message! (concatenate 'string
				   "The "
				   (get-creature-name attacker)
				   " "
				   desc))))

(defmethod cmb-describe-hit (attacker target the-attack)
  (declare (ignore the-attack))
  (c-print-message! (format nil "~a hits the ~a."
			    (get-creature-name attacker)
			    (get-creature-name target)))
  nil)

(defmethod cmb-describe-death (attacker target)
  (declare (ignore attacker))
  (c-print-message! (format nil "The ~a dies.."
			    (get-creature-name target)))
  nil)

(defun attack-target! (dun attacker target x y the-attack)
  (play-sound 1)
  ;;	(describe the-monster)
  (if (not (cmb-hit-creature? attacker target the-attack))
      (cmb-describe-miss attacker target)
      (progn
	(cmb-describe-hit attacker target the-attack)
	(cmb-inflict-damage! attacker target the-attack)
	      	      
	(when (< (current-hp target) 0)
	  (cmb-describe-death attacker target)
	  (let ((target-xp (get-xp-value target)))
	    (increase-xp! attacker (if target-xp target-xp 0)))
		
	  (kill-target! dun x y target)
	  ;; repaint
	  (light-spot! dun x y)
	  ))))



(defun attack-location! (dun pl x y)
  "attacks a given location.."

  (let ((monsters (cave-monsters dun x y)))
    (when monsters
      (let ((the-monster (car monsters)))
	;; hack
	(play-sound 1)
	(attack-target! dun pl the-monster x y nil)
	))))


(defun cmb-monster-attack! (dun pl mon the-x the-y)
  "The monster attacks the player (pl) at (the-x,the-y)."
  (dolist (the-attack (monster.attacks mon))
    (when (and (creature-alive? mon) (creature-alive? pl))
      (attack-target! dun mon pl the-x the-y the-attack))))

#||
  (let ((mon-name (monster.name mon)))
  
    (if (cmb-hit-creature? mon pl)
	(warn "'~a' hit the player.." mon-name)
	(warn "'~a' missed the player.." mon-name))
    ))
||#  