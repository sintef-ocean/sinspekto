(defun my/return-t (orig-fun &rest args)
t)
(defun my/disable-yornp (orig-fun &rest args)
  (advice-add 'yes-or-no-p :around #'my/return-t)
  (advice-add 'y-or-n-p :around #'my/return-t)
  (let ((res (apply orig-fun args)))
    (advice-remove 'yes-or-no-p #'my/return-t)
    (advice-remove 'y-or-n-p #'my/return-t)
    res))
(advice-add 'org-babel-tangle :around #'my/disable-yornp)

;(setq out-dir "/tmp/orgie2")


(defun org-export-output-file-name-modified
    (orig-fun extension &optional subtreep pub-dir)
  (unless pub-dir
    (setq pub-dir out-dir)
    (unless (file-directory-p pub-dir)
      (make-directory pub-dir)))
  (apply orig-fun extension subtreep pub-dir nil))

(advice-add 'org-export-output-file-name
            :around #'org-export-output-file-name-modified)


(package-initialize)

;(add-to-list 'load-path ".")
;(require 'emacs-install-pkgs)

(if (eq system-type 'windows-nt)
  (setq org-plantuml-jar-path "C:/ProgramData/chocolatey/lib/plantuml/tools/plantuml.jar")
  (setq org-plantuml-jar-path "/usr/share/plantuml/plantuml.jar")
)

(setq plantuml-default-exec-mode 'jar)
(setq my-languages '((shell . t)
                     (python . t)
                     (plantuml . t)
                     (C . t)))
(org-babel-do-load-languages 'org-babel-load-languages my-languages)
(setq auto-save-default nil make-backup-files nil)

;; Disable a lot of export parameters,https://orgmode.org/manual/Export-Settings.html
(setq org-export-with-broken-links t)
(setq org-export-with-author nil)
(setq org-export-with-date nil)
(setq org-export-with-email nil)
(setq org-export-with-section-numbers nil)
(setq org-export-with-toc nil)


(require 'org)
(require 'ox-rst)

(defun jemacs-export-org-doc ()
  (dolist (file command-line-args-left)
    (princ (concat "Exporting " file " to " out-dir "/\n") #'external-debugging-output)
    (with-current-buffer (find-file-noselect file)
      (org-rst-export-to-rst))))
(advice-add 'jemacs-export-org-doc :around #'my/disable-yornp)
